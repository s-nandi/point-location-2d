#include "point_location/walking/walking_point_location.h"
#include "planar_structure/plane.h"
#include <memory>

walking_point_location::walking_point_location(std::unique_ptr <walking_scheme> &loc, std::unique_ptr <starting_edge_selector> &sel)
{
    locator = std::move(loc);
    selector = std::move(sel);
}

void walking_point_location::init(plane &pln)
{
    selector -> setPlane(pln);
}

void walking_point_location::addEdge(edge* e)
{
    selector -> addEdge(e);
}

void walking_point_location::removeEdge(edge* e)
{
    selector -> removeEdge(e);
}

edge* walking_point_location::locate(point p)
{
    edge* start = selector -> getStartingEdge(p);
    edge* located = locator -> locate(start, p);
    selector -> locatedEdge(located);
    return located;
}
