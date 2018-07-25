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
    int time = 1;
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
        edges[i] -> setEndpoints(vertices[i], vertices[inext], face, &extremeVertex);
    }
    for (int i = 0; i < vertices.size(); i++)
    {
        int inext = nextIndex(i, vertices.size());
        splice(edges[inext], edges[i] -> twin());
    }
    incidentEdge = edges[0];
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

struct compare_by_endpoint_indices
{
    bool operator () (const edge* a, const edge* b) const
    {
        int v1 = a -> getOrigin() -> label, v2 = a -> getDest() -> label;
        int v3 = b -> getOrigin() -> label, v4 = b -> getDest() -> label;
        return std::make_pair(std::min(v1, v2), std::max(v1, v2)) < std::make_pair(std::min(v3, v4), std::max(v3, v4));
    }
};

struct compare_by_endpoints
{
    bool operator () (const edge* a, const edge* b) const
    {
        point v1 = a -> getOrigin() -> p, v2 = a -> getDest() -> p;
        point v3 = b -> getOrigin() -> p, v4 = b -> getDest() -> p;
        return std::make_pair(std::min(v1, v2), std::max(v1, v2)) < std::make_pair(std::min(v3, v4), std::max(v3, v4));
    }
};

// Assumes that all points are distinct and that the faces are given in ccw  order
edge* plane::init_subdivision(const std::vector <point> &points, const std::vector <std::vector<int>> &faces)
{
    std::vector <vertex*> vertices(points.size());
    for (int i = 0; i < points.size(); i++)
    {
        vertices[i] = new vertex(points[i], i); // change to new vertex(points[i], i) when done
    }

    std::vector <edge*> edges;
    for (int i = 0; i < faces.size(); i++)
    {
        std::vector <vertex*> face_vertices;
        for (int vertex_index: faces[i])
        {
            face_vertices.push_back(vertices[vertex_index]);
        }
        edge* startingEdge = init_polygon(face_vertices, i + 1);
        std::vector <edge*> face_edges = incidentToFace(startingEdge);
        // Push constructed edges to edges vector so that duplicated edges can be merged later
        for (edge* e: face_edges)
        {
            edges.push_back(e);
        }
        incidentEdge = startingEdge;
    }

    // After sorting, duplicate edges will be next to each other
    std::sort(edges.begin(), edges.end(), compare_by_endpoints());
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
            std::cout<<*edges[i]<<" is a boundary edge"<<std::endl;
            edge* inward = edges[i] -> rot();
            inward -> setEndpoints(&extremeVertex, inward -> getDest());
            i++;
        }
    }

    //connectToExterior(boundary_edge);

    return incidentEdge;
}

/* Plane Traversal */

void traverseEdgeDfs(edge* curr, std::vector <edge*> &res, int timestamp)
{
    //std::cout<<"At: "<<*curr<<'\n';;
    bool unused = curr -> getParent() -> use(timestamp);
    if (!unused) return;
    res.push_back(curr);

    std::vector <edge*> incident = incidentToOrigin(curr -> twin());

    std::cout<<"Curr: "<<*curr<<std::endl;
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
    //std::cout<<"At: "<<*curr<<std::endl;
    bool unused = curr -> getOrigin() -> use(timestamp);
    if (!unused) return;
    res.push_back(curr);

    std::cout<<"BEfore"<<std::endl;
    std::cout<<"Curr: "<<*curr<<std::endl;
    std::vector <edge*> incident = incidentToOrigin(curr);
    std::cout<<"AfteR"<<std::endl;

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
    std::cout<<"Time before facetraversal: "<<p.time<<std::endl;
    auto faces = p.traverseFaces(traversalMode::useEdgeOnce);
    std::cout<<"Face Traversal " << faces.size() <<std::endl;
    for (edge* f: faces)
    {
        std::cout<<"Face: "<< std::endl;
        std::cout<<*f<< std::endl;
        auto around = incidentToFace(f -> rot());
        std::cout<<"Around: "<< " "<< around.size() << std::endl;
        for (edge* ar: around)
        {
            std::cout<<*ar<< std::endl;
        }
    }
    std::cout<<"Time before vertextraversal: "<<p.time<<std::endl;
    auto vertices = p.traverseVertices(traversalMode::useVertexOnce);
    std::cout<<"Vertex Traversal"<<std::endl;
    for (edge* v: vertices)
    {
        std::cout<<"Vertex: "<< std::endl;
        std::cout<<v -> origin()<<" | prev: " << v -> destination() << std::endl;
        auto around = incidentToFace(v -> rot());
        std::cout<<"Around: "<<std::endl;
        for (edge* ar: around)
        {
            std::cout<<*ar<<std::endl;
        }
    }
}

void debug_quiet(std::ostream &os, plane &p)
{
    auto faces = p.traverseFaces(traversalMode::useEdgeOnce);
    std::cout<<"Face Traversal"<<std::endl;
    for (edge* f: faces)
    {
        std::cout<<*f<<'\n';
    }
    auto vertices = p.traverseVertices(traversalMode::useVertexOnce);
    std::cout<<"Vertex Traversal"<<std::endl;
    for (edge* v: vertices)
    {
        std::cout<<v -> origin()<<" | prev: " << v -> destination() << '\n';
    }
}

#endif
