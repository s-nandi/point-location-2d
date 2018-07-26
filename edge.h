#ifndef EDGE_H_DEFINED
#define EDGE_H_DEFINED

#include "vertex.h"

class quadedge;

struct edge
{
friend quadedge;
private:
    int type = -1;
    edge *next = NULL;
    vertex *orig = NULL;
    quadedge *par = NULL;

public:
    edge* rot() const;
    edge* invrot() const;
    edge* twin() const;
    edge* onext() const;
    edge* oprev() const;
    edge* fnext() const;
    edge* fprev() const;

    void setRot(edge*&);
    void setTwin(edge*&);
    void setNext(edge* &e) {next = e;}
    quadedge* getParent() const {return par;}

    vertex* getOrigin() const;
    vertex* getDest() const;
    vertex origin() const;
    vertex destination() const;
    void setEndpoints(vertex*, vertex*, vertex*, vertex*);

    bool sameEndpoints(edge*);
    bool flippedEndpoints(edge*);

    friend void splice(edge*, edge*);

    friend std::ostream& operator << (std::ostream&, const edge&);
};

std::ostream& operator << (std::ostream &os, const edge &e)
{
    return os << e.origin() << " to " << e.destination() << " (" << e.invrot() -> origin() << ", " << e.invrot() -> destination() << ")";
}


#endif
