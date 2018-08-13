#ifndef VERTEX_H_DEFINED
#define VERTEX_H_DEFINED

#include "geo_primitives/point2D.h"

typedef point2D point;

class plane;
class vertex
{
friend plane;
private:
    point position;
    int label;
    bool hasPosition = false;

    bool use(int);
    int lastUsed;
public:
    vertex(){}
    vertex(int);
    vertex(point, int);

    int getLabel() {return label;}
    point getPosition() {return position;}

    friend std::ostream& operator << (std::ostream&, const vertex&);
};

#endif
