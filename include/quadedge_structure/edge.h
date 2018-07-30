#ifndef EDGE_H_DEFINED
#define EDGE_H_DEFINED

#include "quadedge_structure/vertex.h"

/*
* Used as parameter for iterating through incident edges to an edge
* incidentToFace used to iterate through edges with the same left face in ccw order
* incidentToOrigin used to iterate through edges with the same origin in ccw order
* incidentToDestination used to iterate through edges with the same destination in ccw order
* incidentToEdge used to iterate through edges with the same endpoints in arbitrary order
* ccw ordering is flipped to cw ordering for edges belonging to the outside face
*/
enum incidenceMode
{
    incidentToFace,
    incidentToOrigin,
    incidentToDestination,
    incidentToEdge
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

    vertex* getOrigin() const;
    vertex* getDest() const;
    void setEndpoints(vertex* = NULL, vertex* = NULL, vertex* = NULL, vertex* = NULL);

    friend void splice(edge*, edge*);
    friend void deleteEdge(edge*);
    friend edge* connect(edge*, edge*, int);
    friend edge* mergeTwins(edge* a, edge* b);
public:
    vertex origin() const;
    vertex destination() const;
    vertex leftface() const;
    vertex rightface() const;

    edge* rot() const;
    edge* invrot() const;
    edge* twin() const;
    edge* onext() const;
    edge* oprev() const;
    edge* fnext() const;
    edge* fprev() const;

    struct iterator;
    iterator begin(incidenceMode = incidentToFace);
    iterator end(incidenceMode = incidentToFace);

    friend std::ostream& operator << (std::ostream&, const edge&);
};

#endif
