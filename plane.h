#ifndef PLANE_H_DEFINED
#define PLANE_H_DEFINED

#include <vector>
#include <algorithm>
#include "quadedge.h"

struct plane
{
private:
    edge *incidentEdge;
public:
    plane(){}

    edge* init_polygon(std::vector <vertex*>&, int);
    edge* init_polygon(const std::vector <vertex>&, int);
    edge* init_triangle(vertex&, vertex&, vertex&, int);

    edge* init_subdivision(const std::vector <point>&, const std::vector<std::vector<int>>&);
    friend void debug(std::ostream&, plane&);
};

/* Output planar subdivision */

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

/* Plane construction */

// Assumes points are given in ccw order
edge* plane::init_polygon(std::vector <vertex*> &vertices, int face_number = -1)
{
    std::vector <edge*> edges(vertices.size());
    for (int i = 0; i < vertices.size(); i++)
    {
        int inext = (i + 1 < vertices.size()) ? i + 1 : 0;
        edges[i] = makeEdge();
        edges[i] -> setEndpoints(vertices[i], vertices[inext], face_number);
    }
    for (int i = 0; i < vertices.size(); i++)
    {
        int inext = (i + 1 < vertices.size()) ? i + 1 : 0;
        splice(edges[inext], edges[i] -> twin());
    }
    incidentEdge = edges[0];
    return incidentEdge;
}

edge* plane::init_polygon(const std::vector <vertex> &vertices, int face_number = -1)
{
    std::vector <vertex*> vertices_copy(vertices.size());
    for (int i = 0; i < vertices.size(); i++)
    {
        vertices_copy[i] = new vertex(vertices[i].p);
    }
    return init_polygon(vertices_copy, face_number);
}

edge* plane::init_triangle(vertex &a, vertex &b, vertex &c, int face_number = -1)
{
    std::vector <vertex*> vertices = {&a, &b, &c};
    if (doubleOrientedArea(a.p, b.p, c.p) > 0)
    {
        std::swap(vertices[0], vertices[2]);
    }
    init_polygon(vertices, face_number);
}

// Assumes that all points are distinct and that the faces are given in ccw  order
edge* plane::init_subdivision(const std::vector <point> &points, const std::vector <std::vector<int>> &faces)
{
    edge* returnEdge;

    std::vector <vertex*> vertices(points.size());
    for (int i = 0; i < points.size(); i++)
    {
        vertices[i] = new vertex(points[i], i);
    }
    std::vector <std::vector<edge*>> bucketByOrigin(points.size());
    for (int i = 0; i < faces.size(); i++)
    {
        std::vector <vertex*> face;
        for (int vertex_index: faces[i])
        {
            face.push_back(vertices[vertex_index]);
        }
        edge* startingEdge = init_polygon(face, i);
        std::vector <edge*> faceEdges = incidentToFace(startingEdge);
        for (int j = 0; j < faceEdges.size(); j++)
        {
            std::cout<< "Edge: " << faceEdges[j] -> origin() << " to " << faceEdges[j] -> destination() << '\n';
            bucketByOrigin[faceEdges[j] -> getOrigin() -> label].push_back(faceEdges[j]);
            //bucketByOrigin[faceEdges[j] -> getDest() -> label].push_back(faceEdges[j] -> twin());
        }

        returnEdge = startingEdge;
    }

    for (auto bucket: bucketByOrigin)
    {
        std::sort(bucket.begin(), bucket.end());
        bucket.erase(std::unique(bucket.begin(), bucket.end()), bucket.end());
        if (bucket.size() >= 2)
        {
            for (int i = 1; i < bucket.size(); i++)
            {
                splice(bucket[i - 1], bucket[i]);
            }
        }
    }

    return returnEdge;
}

/* Debugging information */
void debug(std::ostream &os, plane &p)
{
    edge* startingEdge = p.incidentEdge;
    os << "Starting edge: " << startingEdge -> origin() << " to " << startingEdge -> destination() << '\n';

    os << "Points touching origin:" << '\n';
    std::vector <edge*> origin_edges = incidentToOrigin(startingEdge);
    for (auto e: origin_edges)
    {
        os << e -> origin() << " to " << e -> destination() << '\n';
    }

    os << "Points on face (with left and right face labels):" << '\n';
    std::vector <edge*> face_edges = incidentToFace(startingEdge);
    for (auto e: face_edges)
    {
        os << e -> origin() << " " << e -> invrot() -> origin() << " " << e -> rot() -> origin() << '\n';
    }
}

#endif
