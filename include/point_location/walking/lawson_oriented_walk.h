#ifndef LAWSON_ORIENTED_WALK_H_DEFINED
#define LAWSON_ORIENTED_WALK_H_DEFINED

#include <vector>
#include <unordered_set>
#include "point_location/point_location.h"

enum lawsonWalkOptions
{
    stochasticWalk,
    rememberingWalk,
    fastRememberingWalk,
    recentStart,
    sampleStart
};

class point2D;
typedef point2D point;
class plane;
class edge;

class lawson_oriented_walk : public pointlocation
{
private:
    bool isStochastic = false, isRemembering = false, isFast = false;
    bool isRecent = false, isSample = false;
    unsigned int maxFastSteps = 0, sampleSize = 0;
    edge* recentEdge;

    std::vector <edge*> edgeList;
    std::unordered_set <edge*> validEdges;
public:
    int numTests = 0, numFaces = 0;

    void init(plane&);
    void setParameters(const std::vector <lawsonWalkOptions>& = {});
    void setFastSteps(unsigned int);
    void setSampleSize(unsigned int);

    void addEdge(edge*);
    void removeEdge(edge*);
    edge* locate(point);
    edge* bestFromSample(point);
};

#endif
