#ifndef EDGE_H_DEFINED
#define EDGE_H_DEFINED

#include "quadedge_structure/vertex.h"

/*
* Used as parameter for iterating through incident edges to an edge
* incidentOnFace used to iterate through edges with the same left face in ccw order
* incidentToOrigin used to iterate through edges with the same origin in ccw order
* ccw ordering is flipped to cw ordering for edges belonging to the outside face
*/
enum incidenceMode
{
    incidentOnFace,
    incidentToOrigin,
};

class vertex;
class quadedge;
class plane;
class triangulation;

class edge
{
friend quadedge;
friend plane;
friend triangulation;
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
    void labelFace(vertex*);
public:
    vertex& origin() const;
    vertex& destination() const;
    vertex& leftface() const;
    vertex& rightface() const;

    point originPosition() const;
    point destinationPosition() const;
    int leftfaceLabel() const;
    int rightfaceLabel() const;

    edge* rot() const;
    edge* invrot() const;
    edge* twin() const;
    edge* onext() const;
    edge* oprev() const;
    edge* fnext() const;
    edge* fprev() const;

    friend void splice(edge*, edge*);
    friend void deleteEdge(edge*);
    friend edge* connect(edge*, edge*, int = -1);
    friend edge* mergeTwins(edge*, edge*);
    friend edge* rotateInEnclosing(edge*);

    struct iterator;
    iterator begin(incidenceMode = incidentOnFace);
    iterator end(incidenceMode = incidentOnFace);
    iterator rbegin(incidenceMode = incidentOnFace);
    iterator rend(incidenceMode = incidentOnFace);

    friend std::ostream& operator << (std::ostream&, const edge&);
};

#endif
