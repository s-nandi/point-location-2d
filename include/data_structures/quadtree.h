#ifndef QUADTREE_H_DEFINED
#define QUADTREE_H_DEFINED

#include <vector>
#include <tuple>
#include "quadedge_structure/vertex.h"

class edge;

class quadtree
{
private:
    quadtree* children[4];
    long long left, top, right, bottom;
    std::vector <edge*> faces;
    int level;

    int MAX_OVERLAP, MAX_DEPTH;

    bool contains(const point&);
    bool overlaps(edge* face);
    void split();
public:
    quadtree(){}
    quadtree(const std::tuple <T, T, T, T>& bounding_box, int lev = 0);
    void setParameters(int, int);

    void insert(edge* face);
    edge* locate(const point &p);

    int getNumNodes();
    int getDepth();
};

#endif
