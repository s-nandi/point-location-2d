#ifndef QUADEDGE_H_DEFINED
#define QUADEDGE_H_DEFINED

#include "quadedge_structure/edge.h"
#include "quadedge_structure/edge_iterator.h"

class plane;

class quadedge
{
friend edge;
friend plane;
private:
    edge* e[4];
    int lastUsed = -1;

    quadedge();
    ~quadedge();

    edge* getEdge(int i) const {return e[i];}
    void setEdge(int i, edge* ed) {e[i] = ed;}

    friend edge* makeEdge();
    friend void deleteEdge(edge*);
    friend edge* mergeTwins(edge*, edge*);

    bool use(int);
};

edge* makeEdge();

#endif
