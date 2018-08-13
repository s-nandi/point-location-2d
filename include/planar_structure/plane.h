#ifndef PLANE_H_DEFINED
#define PLANE_H_DEFINED

#include "quadedge_structure/quadedge.h"
#include <vector>
#include <iostream>
#include <tuple>

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

class pointlocation;

class plane
{
friend pointlocation;
private:
    static int nextIndex(int, int);
    static bool sameEndpoints(edge*, edge*);
    static bool flippedEndpoints(edge*, edge*);

    static std::vector <edge*> traverseEdgeDfs(edge*, int);
    static std::vector <edge*> traverseVertexDfs(edge*, int);
protected:
    using box = std::tuple<T, T, T, T>;

    static int time;
    static const int INF; // Used as default value of infinity for bounding box
    static vertex extremeVertex;

    edge *incidentEdge = NULL;

    static edge* make_polygon(std::vector <vertex*>&, int);
    static box calculate_LTRB_bounding_box(std::vector <point>&);

    edge* init_polygon(const std::vector <point>&);
    edge* init_bounding_box(const box&);
    edge* init_subdivision(const std::vector <point>&, const std::vector<std::vector<int>>&);
public:
    box bounds;
    std::vector <edge*> traverse(graphType, traversalMode);

    void read_OFF_file(std::istream&);
    void interactiveTour(std::istream&, std::ostream&);
};

#endif
