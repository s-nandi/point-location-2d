#ifndef PLANE_H_DEFINED
#define PLANE_H_DEFINED

#include "quadedge_structure/quadedge.h"
#include <vector>
#include <iostream>

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
friend debug;
private:
    int time = 1;

    static int nextIndex(int, int);
    static bool sameEndpoints(edge*, edge*);
    static bool flippedEndpoints(edge*, edge*);

    static void traverseEdgeDfs(edge*, std::vector <edge*>&, int);
    static void traverseVertexDfs(edge*, std::vector <edge*>&, int);
protected:
    static vertex extremeVertex;
    edge *incidentEdge = NULL;

    static edge* make_polygon(std::vector <vertex*>&, int);
    edge* init_polygon(const std::vector <point>&);
    edge* init_bounding_box(T, T, T, T);
    edge* init_subdivision(const std::vector <point>&, const std::vector<std::vector<int>>&);
public:
    plane(){}
    std::vector <edge*> traverse(graphType, traversalMode);

    void read_OFF_file(std::istream&);
    void interactiveTour(std::istream&, std::ostream&);
};

#endif
