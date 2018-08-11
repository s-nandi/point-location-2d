#include "point_location/walking/starting_edge_selector.h"
#include "planar_structure/plane.h"
#include <random>
#include <ctime>
#include <assert.h>

starting_edge_selector::starting_edge_selector(selectorMode sm, unsigned int sampleSize)
{
    setParameters(sm, sampleSize);
}

void starting_edge_selector::setParameters(selectorMode sm, unsigned int sampleSize)
{
    mode = sm;
    this -> sampleSize = sampleSize;
}

void starting_edge_selector::setPlane(plane &pl)
{
    edgeList.clear();
    validEdges.clear();
    for (edge* e: pl.traverse(primalGraph, traverseEdges))
        addEdge(e);
}

void starting_edge_selector::addEdge(edge* e)
{
    edgeList.push_back(e);
    validEdges.insert(e);
}

void starting_edge_selector::removeEdge(edge* e)
{
    validEdges.erase(e);
}

void starting_edge_selector::locatedEdge(edge* e)
{
    recentEdge = e;
}

edge* starting_edge_selector::getStartingEdge(point p)
{
    // Iff not using the best edge out of a sample to start, sampleSize must zero
    assert((mode == selectSample) ^ (sampleSize == 0));

    if (mode == selectRecent and recentEdge != NULL)
        return recentEdge;
    else if(mode == selectSample)
        return bestFromSample(p);
    else
        return *validEdges.begin();
}

edge* starting_edge_selector::bestFromSample(point p)
{
    assert(edgeList.size() > 0);

    std::mt19937 gen{static_cast<unsigned int>(time(0))};
    std::uniform_int_distribution <int> dist(0, edgeList.size() - 1);

    edge* closestEdge = NULL;
    T distToClosest;
    for (int i = 0; i < sampleSize; i++)
    {
        bool foundEdge = false;
        edge* curr;
        while (!foundEdge)
        {
            int randomIndex = dist(gen);
            if (validEdges.count(edgeList[randomIndex]) == 1)
            {
                curr = edgeList[randomIndex];
                foundEdge = true;
            }
        }

        point a = curr -> originPosition();
        point b = curr -> destinationPosition();
        point midpt = (a + b) / 2;
        T distSq = (midpt - p).magnitudeSquared();
        if (i == 0 or distSq < distToClosest)
        {
            closestEdge = curr;
            distToClosest = distSq;
        }
    }
    return closestEdge;
}
