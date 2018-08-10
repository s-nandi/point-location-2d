#ifndef STARTING_EDGE_SELECTOR_H_DEFINED
#define STARTING_EDGE_SELECTOR_H_DEFINED

#include <vector>
#include <unordered_set>

class point2D;
typedef point2D point;
class edge;
class plane;

enum selectorMode
{
    selectFirst,
    selectRecent,
    selectSample
};

class starting_edge_selector
{
private:
    edge *recentEdge = NULL;
    std::vector <edge*> edgeList;
    std::unordered_set <edge*> validEdges;

    edge* bestFromSample(point);
public:
    selectorMode mode = selectFirst;
    unsigned int sampleSize = 0;

    starting_edge_selector(){}
    starting_edge_selector(selectorMode, unsigned int = 0);
    void setParameters(selectorMode, unsigned int = 0);
    void setPlane(plane&);

    void addEdge(edge*);
    void removeEdge(edge*);
    void locatedEdge(edge*);

    edge* getStartingEdge(point p);
};

#endif
