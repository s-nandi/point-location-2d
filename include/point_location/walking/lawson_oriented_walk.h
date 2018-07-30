#ifndef LAWSON_ORIENTED_WALK_H_DEFINED
#define LAWSON_ORIENTED_WALK_H_DEFINED

#include <vector>
#include "quadedge_structure/plane.h"
#include "point_location/point_location.h"

enum lawsonWalkOptions
{
    stochasticWalk,
    rememberingWalk,
    fastRememberingWalk
};

class plane;
class edge;

class lawson_oriented_walk : public pointlocation
{
private:
    bool isStochastic;
    bool isRemembering;
    bool isFast;
    int maxFastSteps;
public:
    lawson_oriented_walk(plane&, const std::vector <lawsonWalkOptions>& = {}, int = 0);
    edge* locate(point p);
};

#endif
