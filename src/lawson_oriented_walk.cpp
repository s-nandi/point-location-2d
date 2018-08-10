#include "point_location/walking/lawson_oriented_walk.h"
#include "planar_structure/plane.h"
#include <random>
#include <algorithm>
#include <assert.h>
#include <ctime>

lawson_oriented_walk::lawson_oriented_walk(const std::vector <lawsonWalkOptions> &options, unsigned int fastSteps)
{
    setParameters(options, fastSteps);
}

/*
* options can be passed in to modify lawsons original oriented walk algorithm in the following ways:
* stochastic walk processes edges in a face in a random order which prevents infinite loops in certain non-delaunay triangulations, but redundant for delaunay triangulations
* remembering walk saves 1 orientation test for every non-starting face since the walk does not have to check the edge shared with the previous face traversed
* fast remembering walk checks only 1 edge for each face initially and if the edge is not a right turn, assumes that the other edge creates a right turn
       since it assumes that the current face is not the target face and that the plane is a triangulation
*      eventually (after fastSteps steps) reverts back to regular (non-fast) behavior to identify the target face
*/
void lawson_oriented_walk::setParameters(const std::vector <lawsonWalkOptions> &options, unsigned int fastSteps)
{
    isStochastic = isRemembering = isFast = false;
    maxFastSteps = fastSteps;
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
        }
    }
}

/*
* Returns pointer to some edge that belongs to the face that contains p
* If multiple faces contain p (if p is on an edge or coincides with a vertex), an arbitrary edge is returned
* Utilizes lawsons oriented walk algorithm: the walk proceeds in the direction of any edge that creates a right turn with query point p
* Returns NULL if p is outside the plane
*/
edge* lawson_oriented_walk::locate(edge* startEdge, point p)
{
    // Iff not a fast walk, maxFastSteps should remain to 0
    assert(isFast ^ (maxFastSteps == 0));

    edge* currEdge = startEdge;
    if (isFast)
    {
        // A fast remembering walk assumes that the current face is not the target face and that plane is a triangulation
        // Only use this for the first fastSteps steps so that the target face is detected eventually
        for (unsigned int fastSteps = 0; fastSteps < maxFastSteps; fastSteps++)
        {
            edge* e1 = currEdge -> fnext();
            edge* e2 = e1 -> fnext();

            auto orient = orientation(e1 -> originPosition(), e1 -> destinationPosition(), p);
            numTests++;
            edge* candidate;
            // If assumption is valid, then if e1 does not make a right turn, then e2 must make a right turn
            if (orient > 0)
                candidate = e1 -> twin();
            else
                candidate = e2 -> twin();
            numFaces++;
            // If e2 is a boundary edge, point p might be outside the plane or the assumption that the current face is not the target face might be incorrect
            // End the fast portion of the fast remembering walk and let the regular walk determine which case is valid
            if (candidate -> rightfaceLabel() == 0)
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
            auto orient = orientation(e.originPosition(), e.destinationPosition(), p);
            numTests++;
            // If p is to the right of e, go to the twin edge on the right face of e
            if (orient > 0)
            {
                if (e.rightfaceLabel() == 0) return NULL;
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

    return currEdge;
}
