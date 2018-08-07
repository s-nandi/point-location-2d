#include "point_location/point_location.h"
#include "planar_structure/plane.h"

void pointlocation::init(plane &pln)
{
    startingEdge = pln.incidentEdge;
}

