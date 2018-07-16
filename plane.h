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
    void connectToExterior(edge*);
    int time, numFaces = 1;
public:
    plane(){}

    edge* init_polygon(std::vector <vertex*>&, int);
    edge* init_triangle(vertex&, vertex&, vertex&, int);
    edge* init_subdivision(const std::vector <point>&, const std::vector<std::vector<int>>&);

    std::vector <edge*> traverseFaces(traversalMode);
    std::vector <edge*> traverseVertices(traversalMode);

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
    incidentEdge = edges[0];
    numFaces = 1;
    return incidentEdge;
}

edge* plane::init_triangle(vertex &a, vertex &b, vertex &c, int face_number = -1)
{
    std::vector <vertex*> vertices = {&a, &b, &c};
    if (doubleOrientedArea(a.p, b.p, c.p) > 0)
    {
        std::swap(vertices[0], vertices[2]);
    }
    return init_polygon(vertices, face_number);
}

struct vertex_pair
{
    std::pair <int, int> indices;
    edge* e;

    vertex_pair(int a, int b, edge* ed)
    {
        indices = {std::min(a, b), std::max(a, b)};
        e = ed;
    }

    bool operator < (const vertex_pair &o) const
    {
        return indices < o.indices;
    }
};

void plane::connectToExterior(edge* boundary_edge)
{
    // stores inward facing edges from exterior in ccw or cw order
    std::vector <edge*> inward;

    edge* startEdge = boundary_edge;
    edge* currEdge = startEdge;
    std::cout<<"Start do while"<<std::endl;
    do
    {
        edge* inEdge = currEdge -> rot();
        inEdge -> setEndpoints(&extremeVertex, inEdge -> getDest());
        inward.push_back(inEdge);

        currEdge = currEdge -> twin() -> onext();
    }
    while (currEdge != startEdge);

    std::cout<<"Inward: "<<std::endl;
    for (edge* e: inward)
    {
        std::cout<<*e<<std::endl;
    }
    /*
    for (int i = 0; i < inward.size(); i++)
    {
        int inext = (i + 1 < inward.size()) ? i + 1 : 0;
        inward[i] -> setNext(*inward[inext]);
    }
    */
}

// Assumes that all points are distinct and that the faces are given in ccw  order
edge* plane::init_subdivision(const std::vector <point> &points, const std::vector <std::vector<int>> &faces)
{
    std::vector <vertex_pair> vertex_pairs;

    // Create vertex objects
    std::vector <vertex*> vertices(points.size());
    for (int i = 0; i < points.size(); i++)
        vertices[i] = new vertex(i);

    for (int i = 0; i < faces.size(); i++)
    {
        std::vector <vertex*> face_vertices;
        for (int vertex_index: faces[i])
        {
            face_vertices.push_back(vertices[vertex_index]);
        }
        // Push constructed edges to vertex_pairs so that duplicated edges can be merged later
        // Edges are ordered based on origin label then destination label
        edge* startingEdge = init_polygon(face_vertices, i + 1);
        std::vector <edge*> face_edges = incidentToFace(startingEdge);
        for (edge* e: face_edges)
        {
            vertex_pairs.push_back({e -> getOrigin() -> label, e -> getDest() -> label, e});
            std::cout<< "Edge: " << e -> origin() << " to " << e -> destination() << std::endl;;
        }
        incidentEdge = startingEdge;
    }

    // After sorting, duplicate edges will be next to each other
    std::sort(vertex_pairs.begin(), vertex_pairs.end());
    edge* boundary_edge;
    for (int i = 0; i < vertex_pairs.size(); )
    {
        edge *e1 = vertex_pairs[i].e;
        if (i + 1 >= vertex_pairs.size()) // If no edges after e1, e1 must not have duplicate and thus is on boundary
        {
            boundary_edge = e1;
            break;
        }

        edge *e2 = vertex_pairs[i + 1].e;
        if (e1 -> sameEndpoints(e2))
        {
            std::cout<<"Joining " << *e1 <<" and " << *(e2 -> twin()) << '\n';
            auto e2_twin = e2 -> twin();
            incidentEdge = mergeDuplicate(e1, e2_twin);
            i += 2;
        }
        else if(e1 -> flippedEndpoints(e2))
        {
            std::cout<<"Joining " << *e1 <<" and " << *e2 << '\n';
            incidentEdge = mergeDuplicate(e1, e2);
            i += 2;
        }
        else // If edge doesn't have a duplicate, it must be a boundary edge
             // Need to merge the origin rings of the edge pointing inwards from the exterior
        {
            incidentEdge = e1;
            edge* inward = e1 -> rot();
            inward -> setEndpoints(&extremeVertex, inward -> getDest());
            i++;
        }
    }

    //connectToExterior(boundary_edge);

    numFaces = faces.size();
    return incidentEdge;
}

/* Plane Traversal */

void traverseEdgeDfs(edge* curr, std::vector <edge*> &res, int timestamp)
{
    bool unused = curr -> getParent() -> use(timestamp);
    if (!unused) return;
    res.push_back(curr);

    std::vector <edge*> incident = incidentToOrigin(curr -> twin());

    std::cout<<"Curr: "<<*curr<<'\n';
    for (edge* e: incident)
    {
        std::cout<<"Debug neighbor: "<<*e<<std::endl;
    }
    for (edge* e: incident)
    {
        traverseEdgeDfs(e, res, timestamp);
    }
}

void traverseVertexDfs(edge* curr, std::vector <edge*> &res, int timestamp)
{
    /*
    std::cout<<"Checking lastUsed of: "<<*(curr -> getOrigin()) <<" which is "<<curr -> getOrigin() -> lastUsed<<" while timestamp is "<<timestamp<<std::endl;
    std::cout<<"Origin: "<<curr -> getOrigin()<<std::endl;
    */
    bool unused = curr -> getOrigin() -> use(timestamp);
    if (!unused) return;
    res.push_back(curr);

    std::vector <edge*> incident = incidentToOrigin(curr);

    std::cout<<"Curr: "<<*curr<<'\n';
    for (edge* e: incident)
    {
        std::cout<<"Debug neighbor: "<<*(e -> twin())<<std::endl;
    }
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
