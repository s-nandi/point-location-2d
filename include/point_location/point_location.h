#ifndef POINT_LOCATION_H_DEFINED
#define POINT_LOCATION_H_DEFINED

class point2D;
typedef point2D point;
class edge;
class plane;

class pointlocation
{
protected:
    plane* pl;
    edge* startingEdge;
public:
    pointlocation(){}
    pointlocation(plane&);

    virtual edge* locate(point) = 0;
};

#endif
