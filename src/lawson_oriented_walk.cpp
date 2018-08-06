#include "point_location/point_location.h"
#include "point_location/walking/lawson_oriented_walk.h"
#include "planar_structure/plane.h"
#include <random>
#include <algorithm>
#include <assert.h>

/*
* options can be passed in to modify lawsons original oriented walk algorithm in the following ways:
* stochastic walk processes edges in a face in a random order which prevents infinite loops in certain non-delaunay triangulations, but redundant for delaunay triangulations
* remembering walk saves 1 orientation test for every non-starting face since the walk does not have to check the edge shared with the previous face traversed
* fast remembering walk checks only 1 edge for each face initially and if the edge is not a right turn, assumes that the other edge creates a right turn
       since it assumes that the current face is not the target face and that the plane is a triangulation
*      eventually (after fastSteps steps) reverts back to regular (non-fast) behavior to identify the target face
*/
lawson_oriented_walk::lawson_oriented_walk(plane &pln, const std::vector <lawsonWalkOptions> &options, unsigned int fastSteps, unsigned int numSample) : numTests(0), numFaces(0), pointlocation(pln)
{
    isStochastic = isRemembering = isFast = isRecent = isSample = false;
    maxFastSteps = fastSteps;
    sampleSize = numSample;
    for (lawsonWalkOptions option: options)
    {
        switch (option)
        {
            case stochasticWalk:
                isStochastic = true;
                break;
            case rememberingWalk:
                isRemembering = true;
                break;
            case fastRememberingWalk:
                isFast = isRemembering = true;
                break;
            case recentStart:
                isRecent = true;
                break;
            case sampleStart:
                isSample = true;
                break;
        }
    }
    // Iff not a fast walk, maxFastSteps should remain to 0
    assert(isFast ^ (maxFastSteps == 0));
    // Iff not using the best edge out of a sample to start, sampleSize must zero
    assert(isSample ^ (sampleSize == 0));
    // Cannot pick two starting edge modes at the same time
    assert(!isSample or !isRecent);
}

/*
* Returns pointer to some edge that belongs to the face that contains p
* If multiple faces contain p (if p is on an edge or coincides with a vertex), an arbitrary edge is returned
* Utilizes lawsons oriented walk algorthm: the walk proceeds in the direction of any edge that creates a right turn with query point p
* Returns NULL if p is outside the plane
*/
edge* lawson_oriented_walk::locate(point p)
{
    edge* currEdge;
    if (isRecent and recentEdge != NULL) currEdge = recentEdge;
    else if(isSample) currEdge = bestFromSample(p);
    else currEdge = startingEdge;

    if (isFast)
    {
        // A fast remembering walk assumes that the current face is not the target face and that plane is a triangulation
        // Only use this for the first fastSteps steps so that the target face is detected eventually
        for (unsigned int fastSteps = 0; fastSteps < maxFastSteps; fastSteps++)
        {
            edge* e1 = currEdge -> fnext();
            edge* e2 = e1 -> fnext();

            auto orient = orientation(e1 -> origin().getPosition(), e1 -> destination().getPosition(), p);
            numTests++;
            edge* candidate;
            // If assumption is valid, then if e1 does not make a right turn, then e2 must make a right turn
            if (orient > 0)
            {
                candidate = e1 -> twin();
            }
            else
            {
                candidate = e2 -> twin();
            }
            numFaces++;
            // If e2 is a boundary edge, point p might be outside the plane or the assumption that the current face is not the target face might be incorrect
            // End the fast portion of the fast remembering walk and let the regular walk determine which case is valid
            if (candidate -> leftface().getLabel() == 0)
                break;
            else
                currEdge = candidate;
        }
    }

    bool firstIteration = true;
    while (true)
    {
        bool rightTurn = false;
        std::vector <edge> face_edges;
        for (auto it = currEdge -> begin(incidentOnFace); it != currEdge -> end(incidentOnFace); ++it)
        {
            // In a remembering walk, the common edge between the current and previous faces is skipped
            // If processing the first triangle, cannot skip any of the face edges since there is no previous face yet
            if (isRemembering and !firstIteration and it == currEdge -> begin(incidentOnFace)) continue;
            face_edges.push_back(*it);
        }

        // Edges of a face must be processed in a random order for a stochastic walk
        // Necessary to break infinite loops for certain non-delaunay triangulations
        if (isStochastic)
            std::random_shuffle(face_edges.begin(), face_edges.end());

        for (int i = 0; i < face_edges.size(); i++)
        {
            edge e = face_edges[i];
            auto orient = orientation(e.origin().getPosition(), e.destination().getPosition(), p);
            numTests++;
            // If p is to the right of e, go to the twin edge on the right face of e
            if (orient > 0)
            {
                if (e.rightface().getLabel() == 0) return NULL;
                else
                {
                    currEdge = e.twin();
                    rightTurn = true;
                    break;
                }
            }
        }
        firstIteration = false;
        numFaces++;

        // If no right turns are made from the face edges to point p, then p must be inside the face
        if (!rightTurn) break;
    }
    if (isRecent)
        recentEdge = currEdge;

    return currEdge;
}

void lawson_oriented_walk::addEdge(edge* e)
{
    if (isSample)
    {
        edgeList.push_back(e);
        validEdges.insert(e);
    }
}

void lawson_oriented_walk::removeEdge(edge* e)
{
    if (isSample)
    {
        validEdges.erase(e);
    }
}

edge* lawson_oriented_walk::bestFromSample(point p)
{
    assert(edgeList.size() > 0);

    std::random_device rd;
    std::mt19937 gen(rd());
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

        point a = curr -> origin().getPosition();
        point b = curr -> destination().getPosition();
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
