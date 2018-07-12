#ifndef EDGE_H_DEFINED
#define EDGE_H_DEFINED

class quadedge;
class vertex;

struct edge
{
friend quadedge;
private:
    int type = -1;
    edge *next = NULL;
    vertex *orig = NULL;
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

    vertex* getOrigin();
    vertex* getDest();
    vertex origin();
    vertex destination();
    void setEndpoints(vertex*, vertex*);
    void setEndpoints(vertex*, vertex*, int);

    friend void splice(edge*, edge*);
};

#endif
