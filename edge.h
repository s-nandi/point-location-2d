#ifndef EDGE_H_DEFINED
#define EDGE_H_DEFINED

#include <assert.h>
#include <iterator>
#include "vertex.h"

/*
* Used as parameter for iterating through incident edges to an edge
* incidentToFace used to iterate through edges with the same left face in ccw order
* incidentToOrigin used to iterate through edges with the same origin in ccw order
* incidentToDestination used to iterate through edges with the same destination in ccw order
* ccw ordering is flipped to cw ordering for edges belonging to the outside face
*/
enum incidenceMode
{
    incidentToFace,
    incidentToOrigin,
    incidentToDestination
};

class quadedge;
class plane;
class debug;

class edge
{
friend quadedge;
friend plane;
friend debug;
private:
    int type = -1;
    edge *next = NULL;
    vertex *orig = NULL;
    quadedge *par = NULL;

    static inline int shift_up_mod4(int, int);
    static inline int shift_down_mod4(int, int);

    void setTwin(edge*&);
    void setNext(edge* &e) {next = e;}
    quadedge* getParent() const {return par;}

    edge* rot() const;
    edge* invrot() const;
    edge* twin() const;
    edge* onext() const;
    edge* oprev() const;
    edge* fnext() const;
    edge* fprev() const;

    vertex* getOrigin() const;
    vertex* getDest() const;
    void setEndpoints(vertex*, vertex*, vertex*, vertex*);

    friend void splice(edge*, edge*);
    friend void deleteEdge(edge*);
    friend edge* connect(edge*, edge*, int);
    friend edge* mergeTwins(edge* a, edge* b);
public:
    vertex origin() const;
    vertex destination() const;
    vertex leftface() const;
    vertex rightface() const;

    struct iterator;
    iterator begin(incidenceMode);
    iterator end(incidenceMode);

    friend std::ostream& operator << (std::ostream&, const edge&);
};

std::ostream& operator << (std::ostream &os, const edge &e)
{
    return os << e.origin() << " to " << e.destination() << " (" << e.invrot() -> origin() << ", " << e.invrot() -> destination() << ")";
}

class edge::iterator : std::forward_iterator_tag
{
private:
    edge *start, *curr;
    bool incremented;
    incidenceMode mode;
public:
    iterator(edge* e, incidenceMode im) : mode(im), incremented(false), start(e), curr(e) {}
    bool operator != (iterator);
    edge& operator * () {return *curr;}
    edge* operator -> () {return &(this -> iterator::operator*());}
    void operator++ ();

    friend iterator edge::begin(incidenceMode);
    friend iterator edge::end(incidenceMode);
};

// Assumes that o is an iterator with the same start and mode as this
bool edge::iterator::operator != (iterator o)
{
    assert(start == o.start and mode == o.mode);
    return incremented != o.incremented or curr != o.curr;
}

// Assumes that iterator has not reached the end of its cycle
void edge::iterator::operator++ ()
{
    assert(!incremented or curr != start);
    switch (mode)
    {
        case incidentToFace:
            curr = curr -> fnext();
            break;
        case incidentToOrigin:
            curr = curr -> onext();
            break;
        case incidentToDestination:
            curr = curr -> twin() -> onext() -> twin();
            break;
    }
    incremented = true;
}

edge::iterator edge::begin(incidenceMode im = incidentToFace)
{
    return iterator(this, im);
}

edge::iterator edge::end(incidenceMode im = incidentToFace)
{
    auto it = iterator(this, im);
    it.incremented = true;
    return it;
}

#endif
