#ifndef PLANE_H_DEFINED
#define PLANE_H_DEFINED

#include <vector>
#include <algorithm>
#include <assert.h>
#include <sstream>
#include <cctype>
#include "quadedge.h"

/*
* Used as parameter for traversing plane
* traverseNodes ensures that every vertex is visited once
* traverseEdges ensures that every edge is visited once
*/
enum traversalMode
{
    traverseNodes,
    traverseEdges
};

/*
* Used as parameter to determine which graph is traversed
* primalGraph results in traversing vertex to vertex edges
* dualGraph results in traversing face to face edges
*/
enum graphType
{
    primalGraph,
    dualGraph
};

class plane
{
friend debug;
private:
    edge *incidentEdge;
    int time = 1;

    static int nextIndex(int, int);
    static bool sameEndpoints(edge*, edge*);
    static bool flippedEndpoints(edge*, edge*);
    static void traverseEdgeDfs(edge*, std::vector <edge>&, int);
    static void traverseVertexDfs(edge*, std::vector <edge>&, int);

    std::vector <edge*> init_polygon(std::vector <vertex*>&, int);
    edge* init_subdivision(const std::vector <point>&, const std::vector<std::vector<int>>&);
public:
    plane(){}

    void read_OFF_file(std::istream&);
    void interactiveTour(std::istream&, std::ostream&);
    std::vector <edge> traverse(graphType, traversalMode);
};

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

/* Plane construction */

// Assumes points are given in ccw order
std::vector <edge*> plane::init_polygon(std::vector <vertex*> &vertices, int face_number)
{
    vertex* face = new vertex(face_number);
    std::vector <edge*> edges(vertices.size());
    for (int i = 0; i < vertices.size(); i++)
    {
        int inext = nextIndex(i, vertices.size());
        edges[i] = makeEdge();
        edges[i] -> setEndpoints(vertices[i], vertices[inext], face);
    }
    for (int i = 0; i < vertices.size(); i++)
    {
        int inext = nextIndex(i, vertices.size());
        splice(edges[inext], edges[i] -> twin());
    }
    return edges;
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
        std::vector <edge*> face_edges = init_polygon(face_vertices, i + 1);
        // Push constructed edges to edges vector so that duplicated edges can be merged later
        for (edge* e: face_edges)
        {
            edges.push_back(e);
        }
        incidentEdge = face_edges[0];
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
            mergeTwins(edges[i], edges[i + 1] -> twin());
            i += 2;
        }
        else if(i + 1 < edges.size() and flippedEndpoints(edges[i], edges[i + 1]))
        {
            mergeTwins(edges[i], edges[i + 1]);
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
void plane::traverseEdgeDfs(edge* curr, std::vector <edge> &result, int timestamp)
{
    bool unused = curr -> getParent() -> use(timestamp);
    if (!unused) return;

    result.push_back(*curr);
    edge* twin = curr -> twin();
    for (auto it = twin -> begin(incidentToOrigin); it != twin -> end(incidentToOrigin); ++it)
        traverseEdgeDfs(&*it, result, timestamp);
}

// Result stores an edge for each vertex in the plane
// Taking the origin of each edge in result will give the vertices
void plane::traverseVertexDfs(edge* curr, std::vector <edge> &result, int timestamp)
{
    bool unused = curr -> getOrigin() -> use(timestamp);
    if (!unused) return;

    result.push_back(*curr);
    for (auto it = curr -> begin(incidentToOrigin); it != curr -> end(incidentToOrigin); ++it)
        traverseVertexDfs(it -> twin(), result, timestamp);
}

std::vector <edge> plane::traverse(graphType gm, traversalMode tm)
{
    edge* startingEdge;
    if (gm == primalGraph)
        startingEdge = incidentEdge;
    else if(gm == dualGraph)
        startingEdge = incidentEdge -> rot();

    std::vector <edge> result;
    if (tm == traverseEdges)
        traverseEdgeDfs(startingEdge, result, time++);
    else if (tm == traverseNodes)
        traverseVertexDfs(startingEdge, result, time++);
    return result;
}

/* File Parsing */

// Thrown if OFF file does not begin with "OFF"
struct incorrectHeaderException : std::exception
{
    const char * what () const throw ()
    {
    	return "Incorrect File Header: Unable to Parse OFF File";
    }
};

void plane::read_OFF_file(std::istream &is)
{
    int numPoints, numFaces, numEdges;
    std::vector <point> points;
    std::vector <std::vector<int>> faces;

    // Stages: 0 (read header), 1 (read vertex/face/edge count), 2 (read vertices), 3 (read faces)
    int stage = 0;
    // Keeps track of how many lines were read at the current stage
    int linesRead = 0;
    while (stage <= 3)
    {
        std::string line;
        std::getline(is, line);
        std::istringstream iss(line);

        // Ignore comments or blank lines
        if (line.length() == 0 or line[0] == '#')
            continue;

        switch (stage)
        {
            case 0:
            {
                std::string header;
                iss >> header;
                // File must start with OFF
                if (header != "OFF")
                {
                    throw incorrectHeaderException();
                }
                stage = 1;
                break;
            }
            case 1:
            {
                iss >> numPoints >> numFaces >> numEdges;
                points.resize(numPoints);
                faces.resize(numFaces);
                stage = 2;
                break;
            }
            case 2:
            {
                iss >> points[linesRead];
                linesRead++;
                // If numPoints points were read, move to next stage
                if (linesRead == numPoints)
                {
                    stage = 3;
                    linesRead = 0;
                }
                break;
            }
            case 3:
            {
                int n;
                iss >> n;
                faces[linesRead].resize(n);
                for (int i = 0; i < n; i++)
                {
                    iss >> faces[linesRead][i];
                }
                linesRead++;
                // If numFaces faces were read, move to next stage
                if (linesRead == numFaces)
                {
                    stage = 4;
                    linesRead = 0;
                }
                break;
            }
        }
    }

    init_subdivision(points, faces);
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

#endif
