#ifndef WALKING_POINT_LOCATION_H
#define WALKING_POINT_LOCATION_H

#include "point_location/point_location.h"
#include "point_location/walking/starting_edge_selector.h"
#include <memory>

class walking_point_location : public online_point_location
{
private:
    std::unique_ptr <walking_scheme> locator;
    std::unique_ptr <starting_edge_selector> selector;
public:
    walking_point_location(std::unique_ptr<walking_scheme>&, std::unique_ptr<starting_edge_selector>&);

    void init(plane&);
    void addEdge(edge*);
    void removeEdge(edge*);
    edge* locate(point);
};

#endif
