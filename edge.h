#ifndef EDGE_H_DEFINED
#define EDGE_H_DEFINED

class quadedge;
class point;

struct edge
{
friend quadedge;

private:
    int type = -1;
    edge *next = NULL;
    point *orig = NULL;
    quadedge *par = NULL;

public:
    void setNext(edge &e) {next = &e;}
    quadedge* getParent() {return par;}

    edge* rot();
    edge* invrot();
    edge* twin();
    edge* onext();
    edge* oprev();
    edge* fnext();
    edge* fprev();

    point* getOrigin();
    point* getDest();
    point origin();
    point destination();
    void setEndpoints(point*, point*);
};

#endif
