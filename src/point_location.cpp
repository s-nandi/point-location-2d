#include "point_location/point_location.h"
#include "planar_structure/plane.h"

pointlocation::pointlocation(plane &pln) : pl(&pln)
{
    startingEdge = pln.incidentEdge;
}

