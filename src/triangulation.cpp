#include "planar_structure/plane.h"
#include "planar_structure/triangulation.h"
#include "point_location/walking/lawson_oriented_walk.h"
#include "uniform_point_rng.h"
#include "parsing.h"
#include <assert.h>
#include <cmath>

/* Helper function for Calculating Bounding Box */
std::tuple <T, T, T, T> triangulation::calculate_LTRB_bounding_box(std::vector <point> &points)
{
    T left, top, right, bottom;
    for (int i = 0; i < points.size(); i++)
    {
        if (i == 0)
        {
            left = right = points[i].x;
            top = bottom = points[i].y;
        }
        else
        {
            left = std::min(left, points[i].x);
            right = std::max(right, points[i].x);
            top = std::max(top, points[i].y);
            bottom = std::min(bottom, points[i].y);
        }
    }
    return {left, top, right, bottom};
}

edge* triangulation::init_bounding_box(T left, T top, T right, T bottom)
{
    assert(left <= right and bottom <= top);
    edge* e = plane::init_bounding_box(left, top, right, bottom);
    bounded = true;
    // Create a diagonal edge to triangulate the bounding box and split the face
    return connect_split(e -> fnext(), e, 1);
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
void triangulation::addPoint(point p, int index, lawson_oriented_walk &locator, triangulationType type)
{
    edge* located_edge = locator.locate(p);
    for (edge &face_edge: *located_edge)
    {
        point curr_origin = face_edge.origin().getPosition();
        point curr_destination = face_edge.destination().getPosition();
        if (orientation(curr_origin, p, curr_destination) == 0)
        {
            located_edge = &face_edge;
            break;
        }
    }
    point origin = located_edge -> origin().getPosition();
    point destination = located_edge -> destination().getPosition();

    // If p is already an endpoint of an edge, no need to add it again
    if (p == origin or p == destination) return;
    // If p is on edge e, delete e and connect p to its surrounding quadrilateral instead of surrounding triangle
    else if(orientation(origin, p, destination) == 0)
    {
        edge* old_edge = located_edge;
        // Need to set e to oprev since if p were strictly inside face, the new edges would form cw turns w.r.t. the triangle's edges
        // Setting e to e -> oprev() ensures that the new edge will form a cw turn with the newly set e
        located_edge = located_edge -> oprev();
        deleteEdge(old_edge);
        locator.removeEdge(old_edge);
    }

    // Get all edges immediately enclosing point p
    std::vector <edge*> enclosing_edges;
    for (edge &e: *located_edge)
    {
        enclosing_edges.push_back(&e);
    }
    assert(enclosing_edges.size() <= 4);

    edge* new_edge = makeEdge();
    new_edge -> setEndpoints(located_edge -> getOrigin(), new vertex(p, index), located_edge -> invrot() -> getOrigin(), located_edge -> invrot() -> getOrigin());
    splice(new_edge, located_edge);
    locator.addEdge(new_edge);

    // Connect the previously created edge to the following enclosing edge
    // Stop at size - 1 since the last edge connected by this process was created in the previous step
    for (int i = 0; i < enclosing_edges.size() - 1; i++)
    {
        new_edge = connect_split(enclosing_edges[i], new_edge -> twin(), 1);
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

// Helper function used to figure out how many fastSteps we should take or how many edges to sample to pick the starting edge
// Typically set to the 4th or 5th root of the number of vertices
int findCeilNthRoot(int val, int n)
{
    assert(n >= 2 and n <= 10);
    if (val <= 1) return 1;
    for (int i = 2; i <= val; i++)
    {
        int v = 1;
        for (int j = 0; j < n; j++)
        {
            v *= i;
            if (v >= val)
            {
                return i;
            }
        }
    }
    // Should never reach this point since i == val iteration of loop will fulfill v >= val even with j == 0 and will thus return a value
    throw "Failure to terminate";
    return -1;
}

void triangulation::init_triangulation(std::vector <point> &points, triangulationType type)
{
    lawson_oriented_walk locator;
    int numSample = findCeilNthRoot(points.size(), 3); // Optimal sample size is around the cube root of the number of points
    // Stochastic walk is unnecessary for delaunay triangulations, but needed to prevent loops in non-delaunay triangulations
    switch (type)
    {
        case delaunayTriangulation:
        {
            locator = lawson_oriented_walk(*this, {rememberingWalk, sampleStart}, 0, numSample);
            break;
        }
        case arbitraryTriangulation:
        {
            int fastWalkLength = findCeilNthRoot(points.size(), 4);
            locator = lawson_oriented_walk(*this, {fastRememberingWalk, sampleStart}, fastWalkLength, numSample);
            break;
        }
    }

    // If the plane is not bounded already, first create an initialized padded bounding box
    if (!bounded)
    {
        int left, top, right, bottom;
        std::tie(left, top, right, bottom) = calculate_LTRB_bounding_box(points);
        init_bounding_box(left - 1, top + 1, right + 1, bottom - 1);
    }

    // Add bounding box edges to the locator
    for (edge* e: this -> traverse(primalGraph, traverseEdges))
    {
        locator.addEdge(e);
    }

    for (int i = 0; i < points.size(); i++)
    {
        addPoint(points[i], 4 + i, locator, type);
    }

    // Label each left face of the triangulation
    int faceNumber = 1;
    for (edge* e: this -> traverse(dualGraph, traverseNodes))
    {
        // Skip labeling the exterior face (already labelled as the extreme vertex)
        if (e -> origin().getLabel() == 0) continue;

        vertex* face = new vertex(faceNumber);
        e -> rot() -> labelFace(face);
        faceNumber++;
    }
}

void triangulation::generateRandomTriangulation(int numPoints, triangulationType type, T left, T top, T right, T bottom)
{
    // Pad out the bounding box so that we can generate random numbers inclusively in ranges [left, right] and [bottom, top] without letting points fall on the boundary
    init_bounding_box(left - 1, top + 1, right + 1, bottom - 1);

    uniform_point_rng pointRng(left, top, right, bottom);
    std::vector <point> points = pointRng.getRandom(numPoints);

    init_triangulation(points, type);
}

void triangulation::read_PT_file(std::istream &is, triangulationType type)
{
    std::vector <point> points = parse_PT_file(is);
    init_triangulation(points, type);
}
