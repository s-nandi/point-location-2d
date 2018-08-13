#ifndef NAIVE_QUADTREE_H_DEFINED
#define NAIVE_QUADTREE_H_DEFINED

#include <vector>
#include "point_location/point_location.h"
#include "data_structures/quadtree.h"

class naive_quadtree : public point_location
{
private:
    quadtree root;
    int MAX_OVERLAP, MAX_DEPTH;
public:
    naive_quadtree(int overlapBound, int depthBound);

    void init(plane&);
    edge* locate(point);

    std::pair <int, int> getDimensions();
};

#endif
