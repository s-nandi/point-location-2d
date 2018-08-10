#ifndef POINT_LOCATION_H_DEFINED
#define POINT_LOCATION_H_DEFINED

class point2D;
typedef point2D point;
class edge;
class plane;

class point_location
{
public:
    virtual void init(plane&) = 0;
    virtual edge* locate(point) = 0;
};

class walking_scheme
{
public:
    virtual edge* locate(edge*, point) = 0;
};

class online_point_location : public point_location
{
public:
    virtual void addEdge(edge*) = 0;
    virtual void removeEdge(edge*) = 0;
};

#endif
