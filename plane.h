#ifndef PLANE_H_DEFINED
#define PLANE_H_DEFINED

#include <vector>
#include <algorithm>
#include "quadedge.h"
#include "helper.h"

enum traversalMode
{
    useVertexOnce,
    useEdgeOnce
};

struct plane
{
private:
    edge *incidentEdge;
    int time = 1;

    std::vector <edge*> init_polygon(std::vector <vertex*>&, int);
    std::vector <edge*> init_triangle(std::vector <vertex*>&, int);
public:
    plane(){}

    edge* init_subdivision(const std::vector <point>&, const std::vector<std::vector<int>>&);

    std::vector <edge*> traverseFaces(traversalMode);
    std::vector <edge*> traverseVertices(traversalMode);

    friend void debug(std::ostream&, plane&);
    friend void debug_quiet(std::ostream&, plane&);
    friend void debug_tour(std::ostream&, plane&);
};

/* Plane construction */

// Assumes points are given in ccw order
std::vector <edge*> plane::init_polygon(std::vector <vertex*> &vertices, int face_number = -1)
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

std::vector <edge*> plane::init_triangle(std::vector <vertex*> &vertices, int face_number = -1)
{
    if (doubleOrientedArea(vertices[0] -> position, vertices[1] -> position, vertices[2] -> position) > 0)
    {
        std::swap(vertices[0], vertices[2]);
    }
    return init_polygon(vertices, face_number);
}

struct compare_by_endpoint_indices
{
    bool operator () (const edge* a, const edge* b) const
    {
        int v1 = a -> getOrigin() -> label, v2 = a -> getDest() -> label;
        int v3 = b -> getOrigin() -> label, v4 = b -> getDest() -> label;
        return std::make_pair(std::min(v1, v2), std::max(v1, v2)) < std::make_pair(std::min(v3, v4), std::max(v3, v4));
    }
};

// Assumes that all points are distinct and that the faces are given in ccw order
edge* plane::init_subdivision(const std::vector <point> &points, const std::vector <std::vector<int>> &faces)
{
    std::vector <vertex*> vertices(points.size());
    for (int i = 0; i < points.size(); i++)
    {
        vertices[i] = new vertex(i); // change to new vertex(points[i], i) when done
    }

    std::vector <edge*> edges;
    for (int i = 0; i < faces.size(); i++)
    {
        std::vector <vertex*> face_vertices;
        for (int vertex_index: faces[i])
        {
            face_vertices.push_back(vertices[vertex_index]);
        }
        std::vector <edge*> face_edges;
        if (face_vertices.size() == 3)
        {
            face_edges = init_triangle(face_vertices, i + 1);
        }
        else
        {
            face_edges = init_polygon(face_vertices, i + 1);
        }
        // Push constructed edges to edges vector so that duplicated edges can be merged later
        for (edge* e: face_edges)
        {
            edges.push_back(e);
        }
        incidentEdge = face_edges[0];
    }

    // After sorting, duplicate edges will be next to each other
    std::sort(edges.begin(), edges.end(), compare_by_endpoint_indices());
    for (int i = 0; i < edges.size(); )
    {
        if (i + 1 < edges.size() and edges[i] -> sameEndpoints(edges[i + 1]))
        {
            incidentEdge = mergeTwins(edges[i], edges[i + 1] -> twin());
            i += 2;
        }
        else if(i + 1 < edges.size() and edges[i] -> flippedEndpoints(edges[i + 1]))
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

/* Finding incident edges */

// Returns vector consisting of all edges with same origin as edge_from_origin
std::vector <edge*> incidentToOrigin(edge* edge_from_origin)
{
    std::vector <edge*> edges;
    edge* startingEdge = edge_from_origin;
    edge* currEdge = startingEdge;
    do
    {
        edges.push_back(currEdge);
        currEdge = currEdge -> onext();
    }
    while (currEdge != startingEdge);
    return edges;
}

// Returns vector consisting of all edges that have the same left face as edge_on_face
std::vector <edge*> incidentToFace(edge* edge_on_face)
{
    std::vector <edge*> edges;
    edge* startingEdge = edge_on_face;
    edge* currEdge = startingEdge;
    do
    {
        edges.push_back(currEdge);
        currEdge = currEdge -> fnext();
    }
    while (currEdge != startingEdge);
    return edges;
}

/* Plane Traversal */

void traverseEdgeDfs(edge* curr, std::vector <edge*> &res, int timestamp)
{
    bool unused = curr -> getParent() -> use(timestamp);
    if (!unused) return;
    res.push_back(curr);

    std::vector <edge*> incident = incidentToOrigin(curr -> twin());

    for (edge* e: incident)
    {
        traverseEdgeDfs(e, res, timestamp);
    }
}

void traverseVertexDfs(edge* curr, std::vector <edge*> &res, int timestamp)
{
    bool unused = curr -> getOrigin() -> use(timestamp);
    if (!unused) return;
    res.push_back(curr);

    std::vector <edge*> incident = incidentToOrigin(curr);

    for (edge* e: incident)
    {
        traverseVertexDfs(e -> twin(), res, timestamp);
    }
}

std::vector <edge*> plane::traverseFaces(traversalMode mode)
{
    std::vector <edge*> res;
    if (mode == useEdgeOnce) traverseEdgeDfs(incidentEdge -> rot(), res, time++);
    else if (mode == useVertexOnce) traverseVertexDfs(incidentEdge -> invrot(), res, time++);
    return res;
}

std::vector <edge*> plane::traverseVertices(traversalMode mode)
{
    std::vector <edge*> res;
    if (mode == useEdgeOnce) traverseEdgeDfs(incidentEdge, res, time++);
    else if (mode == useVertexOnce) traverseVertexDfs(incidentEdge, res, time++);
    return res;
}

#endif
