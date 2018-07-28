#ifndef LAWSON_ORIENTED_WALK_H_DEFINED
#define LAWSON_ORIENTED_WALK_H_DEFINED

#include <vector>
#include <random>
#include "../../quad_edge/plane.h"
#include "../point_location.h"

struct lawson_oriented_walk : pointlocation
{
    bool isStochastic;
    bool isRemembering;
    lawson_oriented_walk(plane &pln, bool stochastic = false, bool remembering = false) : pointlocation(pln),
                                                                          isStochastic(stochastic),
                                                                          isRemembering(remembering) {}
    edge* locate(point p);
};

/*
* Returns pointer to some edge that belongs to the face that contains p
* If multiple faces contain p (if p is on an edge or coincides with a vertex), an arbitrary edge is returned
* Utilizes lawsons oriented walk algorthm: the walk proceeds in the direction of any edge that creates a right turn with query point p
* Returns NULL if p is outside the plane
*/
edge* lawson_oriented_walk::locate(point p)
{
    bool firstFace = true;
    edge* currEdge = startingEdge;
    std::cout<<"Start: "<< *startingEdge << std::endl;
    while (true)
    {
        bool rightTurn = false;
        std::vector <edge> face_edges;
        for (auto it = currEdge -> begin(incidentToFace); it != currEdge -> end(incidentToFace); ++it)
        {
            // In a remembering walk, the common edge between the current and previous faces is skipped
            // If processing the first triangle, cannot skip any of the face edges since there is no previous face yet
            if (isRemembering and !firstFace and it == currEdge -> begin(incidentToFace)) continue;

            face_edges.push_back(*it);
        }

        // Edges of a face must be processed in a random order for a stochastic walk
        // Necessary to break infinite loops for certain non-delaunay triangulations
        if (isStochastic)
            std::random_shuffle(face_edges.begin(), face_edges.end());

        for (edge &e: face_edges)
        {
            auto orient = orientation(e.origin().getPosition(), e.destination().getPosition(), p);
            numTests++;
            // If p is to the right of e, go to the twin edge on the right face of e
            if (orient > 0)
            {
                if (e.rightface().getLabel() == 0) return NULL;
                else
                {
                    currEdge = &*(++e.begin(incidentToEdge));
                    rightTurn = true;
                    break;
                }
            }
        }

        numFaces++;
        firstFace = false;

        // If no right turns are made from the face edges to point p, then p must be inside the face
        if (!rightTurn) break;
    }
    return currEdge;
}

#endif
