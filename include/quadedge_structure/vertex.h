#ifndef VERTEX_H_DEFINED
#define VERTEX_H_DEFINED

#include "geo_primitives/point2D.h"
#include "geo_primitives/point3D.h"

typedef point2D point;

class plane;

class vertex
{
friend plane;
private:
    int label;
    point position;
    bool hasPosition = false;
    int lastUsed = -1;

    bool use(int);
public:
    vertex(){}
    vertex(int);
    vertex(point, int);

    int getLabel() {return label;}
    point getPosition() {return position;}

    friend std::ostream& operator << (std::ostream&, const vertex&);
};

#endif
