#ifndef LAWSON_ORIENTED_WALK_H_DEFINED
#define LAWSON_ORIENTED_WALK_H_DEFINED

#include <vector>
#include "point_location/point_location.h"

enum lawsonWalkOptions
{
    stochasticWalk,
    rememberingWalk,
    fastRememberingWalk,
};

class lawson_oriented_walk : public walking_scheme
{
private:
    bool isStochastic = false, isRemembering = false, isFast = false;
    unsigned int maxFastSteps = 0;
public:
    int numTests = 0, numFaces = 0;

    lawson_oriented_walk(){}
    lawson_oriented_walk(const std::vector <lawsonWalkOptions>&, unsigned int = 0);
    void setParameters(const std::vector <lawsonWalkOptions>& = {}, unsigned int = 0);

    edge* locate(edge*, point);
};

#endif
