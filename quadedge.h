#ifndef QUADEDGE_H_DEFINED
#define QUADEDGE_H_DEFINED

#include "edge.h"

class quadedge
{
public:
    edge e[4];

    quadedge();
    edge* getEdge(int i) {return &e[i];}
};

quadedge::quadedge()
{
    e[0].type = 0, e[0].setNext(e[0]);
    e[1].type = 1, e[1].setNext(e[3]);
    e[2].type = 2, e[2].setNext(e[2]);
    e[3].type = 3, e[3].setNext(e[1]);
    e[0].par = e[1].par = e[2].par = e[3].par = this;
}

/* Helper functions for edge algebra */

// Assumes start is valid mod 4 and (start + shift) wraps around 4 at most once
inline int shift_up_mod4(int start, int shift)
{
    return (start + shift < 4) ? start + shift : start + shift - 4;
}

// Assumes start is valid mod 4 and (start - shift) wraps around 0 at most once
inline int shift_down_mod4(int start, int shift)
{
    return (start - shift >= 0) ? start - shift : start - shift + 4;
}

/* Edge algebra */

// Returns dual edge pointing from right face/vertex towards left face/vertex (rotated ccw)
edge* edge::rot()
{
    return getParent() -> getEdge(shift_up_mod4(type, 1));
}

// Returns dual edge pointing from left face/vertex towards right face/vertex (rotated cw)
edge* edge::invrot()
{
    return getParent() -> getEdge(shift_down_mod4(type, 1));
}

// Returns flipped edge starting at destination and ending at origin
edge* edge::twin()
{
    return getParent() -> getEdge(shift_up_mod4(type, 2));
}

// Returns next ccw edge around origin
edge* edge::onext()
{
    return next;
}

// Returns next cw edge around origin
edge* edge::oprev()
{
    return rot() -> onext() -> rot();
}

// Returns next (ccw) edge around left face/vertex after current edge
edge* edge::fnext()
{
    return invrot() -> onext() -> rot();
}

// Returns previous (cw) edge around left face/vertex before current edge
edge* edge::fprev()
{
    return invrot() -> oprev() -> rot();
}

/* Endpoint getter/setters */

point* edge::getOrigin()
{
    return orig;
}

point* edge::getDest()
{
    return twin() -> orig;
}

point edge::origin()
{
    return *getOrigin();
}

point edge::destination()
{
    return *getDest();
}

void edge::setEndpoints(point* o, point *d)
{
    orig = o;
    twin() -> orig = d;
}

/* Quadedge Construction */

edge* makeEdge()
{
    quadedge *qe = new quadedge;
    return qe -> getEdge(0);
}



#endif
