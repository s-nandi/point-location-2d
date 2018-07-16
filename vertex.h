#ifndef VERTEX_H_DEFINED
#define VERTEX_H_DEFINED

typedef double T;

#include "point.h"

const int INF = 20;

class vertex
{
public:
    int lastUsed = -1; // Move to private later

    point p;
    int label = -1;
    bool hasPoint = false, hasLabel = false;

    vertex(){}
    vertex(point a) {p = a, hasPoint = true;}

    // -1 labels are ignored (used as default parameter)
    vertex(int i){label = i, hasLabel = (label != -1);}
    vertex(point a, int i) {p = a, label = i; hasPoint = true, hasLabel = (label != -1);}

    T x() const {return p.x;}
    T y() const {return p.y;}

    friend std::ostream& operator << (std::ostream&, const vertex&);

    bool use(int);
    bool isUsed(int);
};
vertex extremeVertex = vertex(0);

// Returns false if vertex was used during or after given timestamp
// Otherwise returns true and sets lastUsed to timestamp
// Used to ensure that each vertex is only used once in traversal
bool vertex::use(int timestamp)
{
    if (timestamp <= lastUsed) return false;
    else
    {
        lastUsed = timestamp;
        return true;
    }
}

std::ostream& operator << (std::ostream &os, const vertex &v)
{
    if (v.hasPoint and v.hasLabel) return os << "[" << v.p << " : " << v.label << "]";
    else if (v.hasPoint) return os << v.p;
    else if (v.label != -1) return os << v.label;
    else return os << "DNE";
}

#endif

