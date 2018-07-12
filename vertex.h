#ifndef VERTEX_H_DEFINED
#define VERTEX_H_DEFINED

typedef double T;

#include "point.h"

class vertex
{
public:
    point p;
    int label = -1;
    bool hasPoint = false, hasLabel = false;

    vertex(){}
    vertex(T a, T b) {p = point(a, b), hasPoint = true;}
    vertex(point a) {p = a, hasPoint = true;}

    // -1 labels are ignored (used as default parameter)
    vertex(int i){label = i, hasLabel = (label != -1);}
    vertex(point a, int i) {p = a, label = i; hasPoint = true, hasLabel = (label != -1);}

    T x() const {return p.x;}
    T y() const {return p.y;}

    friend std::ostream& operator << (std::ostream&, const vertex&);
};

std::ostream& operator << (std::ostream &os, const vertex &v)
{
    if (v.hasPoint and v.hasLabel) return os << "[" << v.p << " : " << v.label << "]";
    else if (v.hasPoint) return os << v.p;
    else if (v.label != -1) return os << v.label;
    else return os << "DNE";
}

#endif

