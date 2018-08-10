#include "planar_structure/plane.h"
#include "planar_structure/triangulation.h"
#include "point_location/point_location.h"
#include "point_location/walking/lawson_oriented_walk.h"
#include "point_location/walking/starting_edge_selector.h"
#include "point_location/walking/walking_point_location.h"
#include "uniform_point_rng.h"
#include "parsing.h"
#include <assert.h>
#include <cmath>
#include <algorithm>
#include <memory>

const int triangulation::INF = 1231231234;

edge* triangulation::init_bounding_box(const box &LTRB)
{
    edge* e = plane::init_bounding_box(LTRB);
    // Create a diagonal edge to triangulate the bounding box and split the face
    return connect(e -> fnext(), e);
}

// Checks if e violates the delaunay condition upon the insertion of point p
// If it does, rotates the edge within its quadtrilateral to fix the condition
// Afterwards checks neighboring edges to see if they now violate the delaunay condition
void triangulation::fixDelaunayCondition(point p, edge* e)
{
    // If e is a boundary edge, it cannot be flipped since it does not have an enclosing quadrilateral
    if (e -> leftface().getLabel() == 0 or e -> rightface().getLabel() == 0) return;
    point a = e -> origin().getPosition();
    point b = e -> destination().getPosition();
    point c = e -> twin() -> fnext() -> destination().getPosition();

    assert(e -> fnext() -> destination().getPosition() == p);
    assert(orientation(a, b, c) > 0);
    // If delaunay condition is violated, swap the offending edge
    if (inCircle(c, a, b, p) > 0)
    {
        edge* fixed_edge = rotateInEnclosing(e);
        numDelaunayFlips++;
        // All flipped edges will be incident to the inserted point p
        assert(fixed_edge -> origin().getPosition() == p or fixed_edge -> destination().getPosition() == p);
        // Need to check if neighbors of the rotated edge need to be fixed
        fixDelaunayCondition(p, fixed_edge -> fprev());
        fixDelaunayCondition(p, fixed_edge -> twin() -> fnext());
    }
}

// Adds point to a triangulation and maintains delaunay property as needed
void triangulation::addPoint(point p, int index, online_point_location &locator, triangulationType type)
{
    edge* located_edge = locator.locate(p);
    for (edge &face_edge: *located_edge)
    {
        point curr_origin = face_edge.originPosition();
        point curr_destination = face_edge.destinationPosition();
        if (orientation(curr_origin, p, curr_destination) == 0)
        {
            located_edge = &face_edge;
            break;
        }
    }
    point origin = located_edge -> originPosition();
    point destination = located_edge -> destinationPosition();

    // If p is already an endpoint of an edge, no need to add it again
    if (p == origin or p == destination) return;
    // If p is on edge e, delete e and connect p to its surrounding quadrilateral instead of surrounding triangle
    else if(orientation(origin, p, destination) == 0)
    {
        edge* old_edge = located_edge;
        // Need to set e to oprev since if p were strictly inside face, the new edges would form cw turns w.r.t. the triangle's edges
        // Setting e to e -> oprev() ensures that the new edge will form a cw turn with the newly set e, maintaining the invariant
        located_edge = located_edge -> oprev();
        deleteEdge(old_edge);
        locator.removeEdge(old_edge);
    }

    // Get all edges immediately enclosing point p
    std::vector <edge*> enclosing_edges;
    for (edge &e: *located_edge)
        enclosing_edges.push_back(&e);
    assert(enclosing_edges.size() <= 4);

    edge* new_edge = makeEdge();
    new_edge -> setEndpoints(located_edge -> getOrigin(), new vertex(p, index), located_edge -> invrot() -> getOrigin(), located_edge -> invrot() -> getOrigin());
    splice(new_edge, located_edge);
    locator.addEdge(new_edge);

    // Connect the previously created edge to the following enclosing edge
    // Stop at size - 1 since the last edge connected by this process was created in the previous step
    for (int i = 0; i < enclosing_edges.size() - 1; i++)
    {
        new_edge = connect(enclosing_edges[i], new_edge -> twin());
        locator.addEdge(new_edge);
    }
    if (type == delaunayTriangulation)
    {
        // Need to flip the enclosing edges if they violate the delaunay condition
        for (int i = 0; i < enclosing_edges.size(); i++)
        {
            fixDelaunayCondition(p, enclosing_edges[i]);
        }
    }
}

void triangulation::init_triangulation(std::vector <point> &points, online_point_location &locator, triangulationType type, const box &LTRB)
{
    // Create bounding box, calculate dimensions if not given
    int left, top, right, bottom;
    if (LTRB == box{0, 0, 0, 0})
        std::tie(left, top, right, bottom) = plane::calculate_LTRB_bounding_box(points);
    else
        std::tie(left, top, right, bottom) = LTRB;
    // Pad out the bounding box so that we can generate random numbers inclusively in ranges [left, right] and [bottom, top] without letting points fall on the boundary
    init_bounding_box(box{left - 1, top + 1, right + 1, bottom - 1});

    // Add bounding box edges to the locator
    locator.init(*this);

    // Randomly order the points for delaunay triangulations to achieve average case behavior
    if (type == delaunayTriangulation)
        std::random_shuffle(points.begin(), points.end());

    for (int i = 0; i < points.size(); i++)
    {
        addPoint(points[i], 4 + i, locator, type);
    }

    // Label each left face of the triangulation
    int faceNumber = 1;
    for (edge* e: this -> traverse(dualGraph, traverseNodes))
    {
        // Skip labeling the exterior face (already labeled as the extreme vertex)
        if (e -> getOrigin() -> getLabel() == 0) continue;

        vertex* face = new vertex(faceNumber);
        e -> rot() -> labelFace(face);
        faceNumber++;
    }
}

walking_point_location triangulation::getDefaultLocator(int numPoints, triangulationType type)
{
    std::vector <lawsonWalkOptions> walkOptions;
    int fastWalk = 0;
    // Stochastic walk is unnecessary for delaunay triangulations, but needed to prevent loops in non-delaunay triangulations
    switch (type)
    {
        case delaunayTriangulation:
            walkOptions = {fastRememberingWalk};
            fastWalk = std::pow(numPoints, 1.0 / 4.0);
            break;
        case arbitraryTriangulation:
            walkOptions = {stochasticWalk, fastRememberingWalk};
            fastWalk = std::pow(numPoints, 1.0 / 4.0);
            break;
    }
    std::unique_ptr <walking_scheme> locator_ptr = std::make_unique<lawson_oriented_walk>(lawson_oriented_walk(walkOptions, fastWalk));
    std::unique_ptr <starting_edge_selector> selector_ptr = std::make_unique<starting_edge_selector>(starting_edge_selector({selectSample}, std::pow(numPoints, 1.0 / 3.0)));
    walking_point_location locator(locator_ptr, selector_ptr);
    return locator;
}

void triangulation::generateRandomTriangulation(int numPoints, triangulationType type, const box &LTRB)
{
    walking_point_location locator = getDefaultLocator(numPoints, type);
    generateRandomTriangulation(numPoints, locator, type, LTRB);
}

void triangulation::generateRandomTriangulation(int numPoints, online_point_location &locator, triangulationType type, const box &LTRB)
{
    T left, top, right, bottom;
    std::tie(left, top, right, bottom) = LTRB;
    uniform_point_rng pointRng(left, top, right, bottom);
    std::vector <point> points = pointRng.getRandom(numPoints);
    init_triangulation(points, locator, type, LTRB);
}

void triangulation::read_PT_file(std::istream &is, triangulationType type)
{
    std::vector <point> points = parse_PT_file(is);
    walking_point_location locator = getDefaultLocator(points.size(), type);
    init_triangulation(points, locator, type);
}
