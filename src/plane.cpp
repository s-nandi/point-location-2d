#include "planar_structure/plane.h"
#include "parsing.h"
#include <algorithm>
#include <tuple>
#include <stack>
#include <assert.h>
#include <cctype>

/*
* Helper function for indexing
* Returns next index after i, rolling back to 0 if next index is out of bounds
*/
int plane::nextIndex(int i, int sz)
{
    return (i + 1 < sz) ? i + 1 : 0;
}

/* Helper function for checking if edges are duplicates */

// Checks if origin and destination addresses of e1 and e2 are same
bool plane::sameEndpoints(edge* e1, edge* e2)
{
    return e1 -> getOrigin() == e2 -> getOrigin() and e1 -> getDest() == e2 -> getDest();
}

// Checks if origin and destination addresses of e1 and e2 are flipped
bool plane::flippedEndpoints(edge* e1, edge* e2)
{
    return e1 -> getOrigin() == e2 -> getDest() and e1 -> getDest() == e2 -> getOrigin();
}

plane::~plane()
{
    std::vector <quadedge*> quadedge_list;
    std::vector <vertex*> vertex_list;
    std::vector <vertex*> face_list;

    for (edge* e: this -> traverse(primalGraph, traverseEdges))
        quadedge_list.push_back(e -> getParent());
    for (edge* e: this -> traverse(primalGraph, traverseNodes))
        vertex_list.push_back(e -> getOrigin());
    for (edge* e: this -> traverse(dualGraph, traverseNodes))
        face_list.push_back(e -> getOrigin());

    for (auto elem: quadedge_list)
        delete elem;
    for (auto elem: vertex_list)
        delete elem;
    for (auto elem: face_list)
        delete elem;
}

/* Plane Construction Helpers */

// extremeVertex is used as the outside face for any edge on the boundary of the plane
vertex plane::extremeVertex = vertex(0);
const int plane::INF = 1231231234;
int plane::time = 1;

/* Helper function for Calculating Bounding Box */
plane::box plane::calculate_LTRB_bounding_box(std::vector <point> &points)
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
    return box{left, top, right, bottom};
}

// Assumes points are given in ccw order
// Creates a polygon with a left face of Face_number and a right face corresponding to the exterior face
edge* plane::make_polygon(std::vector <vertex*> &vertices, int face_number)
{
    vertex* face = new vertex(face_number);
    std::vector <edge*> edges(vertices.size());
    for (int i = 0; i < vertices.size(); i++)
    {
        int inext = nextIndex(i, vertices.size());
        edges[i] = makeEdge();
        edges[i] -> setEndpoints(vertices[i], vertices[inext], face, &extremeVertex);
    }
    for (int i = 0; i < vertices.size(); i++)
    {
        int inext = nextIndex(i, vertices.size());
        splice(edges[inext], edges[i] -> twin());
        // Check that points are in ccw order
        assert(orientation(edges[i] -> originPosition(), edges[i] -> destinationPosition(), edges[inext] -> destinationPosition()) <= 0);
    }
    return edges[0];
}

/* Plane Construction */

edge* plane::init_polygon(const std::vector <point> &points)
{
    std::vector <vertex*> vertices(points.size());
    for (int i = 0; i < vertices.size(); i++)
    {
        vertices[i] = new vertex(points[i], i);
    }
    incidentEdge = make_polygon(vertices, 1);
    return incidentEdge;
}

edge* plane::init_bounding_box(const box &LTRB)
{
    T left, top, right, bottom;
    std::tie(left, top, right, bottom) = LTRB;
    // Check that given LTRB is valid
    assert(left <= right and bottom <= top);
    bounds = LTRB;
    std::vector <point> corners = {{left, top}, {left, bottom}, {right, bottom}, {right, top}};
    return init_polygon(corners);
}

// Assumes that all points are distinct and that the points of each face are given in ccw order
edge* plane::init_subdivision(const std::vector <point> &points, const std::vector <std::vector<int>> &faces)
{
    std::vector <vertex*> vertices(points.size());
    for (int i = 0; i < points.size(); i++)
    {
        vertices[i] = new vertex(points[i], i);
    }
    std::vector <edge*> edges;
    for (int i = 0; i < faces.size(); i++)
    {
        std::vector <vertex*> face_vertices;
        for (int vertex_index: faces[i])
        {
            face_vertices.push_back(vertices[vertex_index]);
        }
        edge* face_edge = make_polygon(face_vertices, i + 1);
        // Push constructed edges to edges vector so that duplicated edges can be merged later
        for (edge &e: *face_edge)
        {
            edges.push_back(&e);
        }
        incidentEdge = face_edge;
    }

    // Comparator for sorting endpoints by their minimum endpoint label (then their maximum endpoint label if needed)
    // Edges with the same or flipped endpoints will be adjacent according to this comparator
    auto compare_by_endpoint_indices = [](const edge* a, const edge* b)
    {
        int v1 = a -> getOrigin() -> label, v2 = a -> getDest() -> label;
        int v3 = b -> getOrigin() -> label, v4 = b -> getDest() -> label;
        return std::make_pair(std::min(v1, v2), std::max(v1, v2)) < std::make_pair(std::min(v3, v4), std::max(v3, v4));
    };

    // After sorting, duplicate edges will be next to each other
    std::sort(edges.begin(), edges.end(), compare_by_endpoint_indices);
    for (int i = 0; i < edges.size(); )
    {
        if (i + 1 < edges.size() and sameEndpoints(edges[i], edges[i + 1]))
        {
            incidentEdge = mergeTwins(edges[i], edges[i + 1] -> twin());
            i += 2;
        }
        else if(i + 1 < edges.size() and flippedEndpoints(edges[i], edges[i + 1]))
        {
            incidentEdge = mergeTwins(edges[i], edges[i + 1]);
            i += 2;
        }
        else // If edge e1 doesn't have a twin, it must be a boundary edge
             // Need to set origin of the edge pointing inwards to the extreme vertex
        {
            incidentEdge = edges[i];
            edges[i] -> rot() -> setEndpoints(&extremeVertex);
            i++;
        }
    }
    return incidentEdge;
}

/* Plane Traversal */

// Result stores every distinct edge in the plane
// Distinct edges are edges that do not belong to the same quadedge
std::vector <edge*> plane::traverseEdgeDfs(edge* firstEdge, int timestamp)
{
    std::vector <edge*> result;
    std::stack <edge*> edge_stack;
    edge_stack.push(firstEdge);
    while (!edge_stack.empty())
    {
        edge* curr = edge_stack.top();
        edge_stack.pop();

        bool unused = curr -> getParent() -> use(timestamp);
        if (!unused) continue;

        result.push_back(curr);
        edge* twin = curr -> twin();
        for (auto it = twin -> begin(incidentToOrigin); it != twin -> end(incidentToOrigin); ++it)
        {
            if (it -> getParent() -> lastUsed < timestamp)
                edge_stack.push(&*it);
        }
    }
    return result;
}

// Result stores an edge for each vertex in the plane
// Taking the origin of each edge in result will give all vertices
std::vector <edge*> plane::traverseVertexDfs(edge* firstEdge, int timestamp)
{
    std::vector <edge*> result;
    std::stack <edge*> edge_stack;
    edge_stack.push(firstEdge);
    while (!edge_stack.empty())
    {
        edge* curr = edge_stack.top();
        edge_stack.pop();

        bool unused = curr -> getOrigin() -> use(timestamp);
        if (!unused) continue;

        result.push_back(curr);
        for (auto it = curr -> begin(incidentToOrigin); it != curr -> end(incidentToOrigin); ++it)
        {
            if (it -> twin() -> getOrigin() -> lastUsed < timestamp)
                edge_stack.push(it -> twin());
        }
    }
    return result;
}

std::vector <edge*> plane::traverse(graphType gm, traversalMode tm)
{
    edge* startingEdge;
    if (gm == primalGraph)
        startingEdge = incidentEdge;
    else if(gm == dualGraph)
        startingEdge = incidentEdge -> rot();

    if (tm == traverseEdges)
        return traverseEdgeDfs(startingEdge, time++);
    else if (tm == traverseNodes)
        return traverseVertexDfs(startingEdge, time++);
}

/* File Input/Output */

void plane::read_OFF_file(std::istream &is)
{
    std::vector <point> points;
    std::vector <std::vector<int>> faces;
    std::tie(points, faces) = parse_OFF_file(is);
    init_subdivision(points, faces);
}

void plane::write_OFF_file(std::ostream &os)
{
    auto primal_edges = traverse(primalGraph, traverseNodes);
    auto dual_edges = traverse(dualGraph, traverseNodes);
    int numPoints = primal_edges.size();
    int numEdges = traverse(primalGraph, traverseEdges).size();
    int numFaces = (int) dual_edges.size() - 1;
    std::vector <point> points(numPoints);
    std::vector <std::vector<int>> faces(numFaces + 1);
    for (edge* e: primal_edges)
    {
        vertex* o = e -> getOrigin();
        points[o -> getLabel()] = o -> getPosition();
    }
    for (edge* e: dual_edges)
    {
        int index = e -> getOrigin() -> getLabel();
        if (index == 0) continue;
        assert(index <= numFaces and index >= 0);
        for (auto it = e -> rot() -> begin(incidentOnFace); it != e -> rot() -> end(incidentOnFace); ++it)
        {
            faces[index].push_back(it -> getOrigin() -> getLabel());
        }
    }

    os << "OFF" << '\n';
    os << numPoints << " " << numFaces << " " << numEdges << '\n';
    for (int i = 0; i < numPoints; i++)
        os << points[i].x << " " << points[i].y << '\n';
    for (int i = 1; i <= numFaces; i++)
    {
        os << faces[i].size();
        for (int pointIndex: faces[i])
            os << " " << pointIndex;
        os << '\n';
    }
}

/* Plane Output */

void plane::interactiveTour(std::istream &is, std::ostream &os)
{
    os << "Tour Started" << std::endl;
    os << "Enter \"h\" in console to pull up the help menu" << std::endl;
    edge* curr = incidentEdge;
    edge *e1, *e2;
    while(true)
    {
        os << "Current edge: " << *curr << std::endl;
        char command;
        is >> command;
        command = std::toupper(command);
        switch (command)
        {
            case 'T':
                curr = curr -> twin();
                break;
            case 'R':
                curr = curr -> rot();
                break;
            case 'I':
                curr = curr -> invrot();
                break;
            case 'F':
                curr = curr -> fnext();
                break;
            case 'B':
                curr = curr -> fprev();
                break;
            case 'O':
                curr = curr -> onext();
                break;
            case 'P':
                curr = curr -> oprev();
                break;
            case 'S':
                curr = incidentEdge;
                break;
            case '1':
                e1 = curr;
                os << '\n' << "Edge 1 set to: " << *e1 << '\n' << std::endl;
                break;
            case '2':
                e2 = curr;
                os << '\n' << "Edge 2 set to: " << *e2 << '\n' << std::endl;
                break;
            case 'J':
            {
                int label;
                os << "What face label do you want to set for the right face?" << std::endl;;
                is >> label;
                curr = connect(e1, e2, label);
                e1 = e2 = NULL;
                break;
            }
            case 'D':
                assert(e1 != curr and e1 != incidentEdge);
                os << "Deleting " << *e1 << std::endl;
                deleteEdge(e1);
                e1 = NULL;
                break;
            case 'C':
                for (int i = 0; i < 300; i++)
                    os << std::endl;
                break;
            case 'H':
                os << std::endl;
                os << "Press: " << std::endl;
                os << "T: to go to reverse edge" << std::endl;
                os << "F: to move forward on the left face in ccw order" << std::endl;
                os << "B: to move backward on the left face in ccw order" << std::endl;
                os << "O: to rotate around the origin in ccw order" << std::endl;
                os << "P: to rotate around the origin in cw order" << std::endl;
                os << "R: to go to the dual edge from the right face to the left face" << std::endl;
                os << "I: to go to the dual edge from the left face to the right face" << std::endl;
                os << "S: to restart the starting edge" << std::endl;
                os << "1: to set the marker for edge 1 to the current edge" << std::endl;
                os << "2: to set the marker for edge 2 to the current edge" << std::endl;
                os << "J: to join edge 1 and edge 2 and go to the new edge (assuming they have the same left face)" << std::endl;
                os << "D: to delete edge 1 from the plane (make sure that edge 1 is not the current edge)" << std::endl;
                os << "C: to clear the screen" << std::endl;
                os << "E: to exit" << std::endl;
                os << std::endl;
                break;
            case 'E':
                break;
            default:
                os << command << " is an invalid command" << std::endl;
        }
        if (command == 'E') break;
    }
}
