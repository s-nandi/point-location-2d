#ifndef QUADEDGE_H_DEFINED
#define QUADEDGE_H_DEFINED

#include "edge.h"

class quadedge
{
friend edge;
private:
    int lastUsed = -1;
    static inline int shift_up_mod4(int, int);
    static inline int shift_down_mod4(int, int);
public:
    edge* e[4];

    quadedge();
    edge* getEdge(int i) {return e[i];}
    void setEdge(int i, edge* &ed) {e[i] = ed;}
    friend edge* makeEdge();

    bool use(int);
};

quadedge::quadedge()
{
    for (int i = 0; i < 4; i++)
    {
        e[i] = new edge;
    }
    e[0] -> type = 0, e[0] -> setNext(e[0]);
    e[1] -> type = 1, e[1] -> setNext(e[3]);
    e[2] -> type = 2, e[2] -> setNext(e[2]);
    e[3] -> type = 3, e[3] -> setNext(e[1]);
    e[0] -> par = e[1] -> par = e[2] -> par = e[3] -> par = this;
}

// Returns false if edge was used during or after given timestamp
// Otherwise returns true and sets lastUsed to timestamp
// Used to ensure that each edge is only used once in traversal
bool quadedge::use(int timestamp)
{
    if (timestamp <= lastUsed) return false;
    else
    {
        lastUsed = timestamp;
        return true;
    }
}

/* Helper functions for edge algebra */

// Assumes start is valid mod 4 and (start + shift) wraps around 4 at most once
inline int quadedge::shift_up_mod4(int start, int shift)
{
    return (start + shift < 4) ? start + shift : start + shift - 4;
}

// Assumes start is valid mod 4 and (start - shift) wraps around 0 at most once
inline int quadedge::shift_down_mod4(int start, int shift)
{
    return (start - shift >= 0) ? start - shift : start - shift + 4;
}

/* Edge algebra */

// Returns dual edge pointing from right face/vertex towards left face/vertex (rotated ccw)
edge* edge::rot() const
{
    return getParent() -> getEdge(quadedge::shift_up_mod4(type, 1));
}

// Returns dual edge pointing from left face/vertex towards right face/vertex (rotated cw)
edge* edge::invrot() const
{
    return getParent() -> getEdge(quadedge::shift_down_mod4(type, 1));
}

// Returns flipped edge starting at destination and ending at origin
edge* edge::twin() const
{
    return getParent() -> getEdge(quadedge::shift_up_mod4(type, 2));
}

// Returns next ccw edge around origin
edge* edge::onext() const
{
    return next;
}

// Returns next cw edge around origin
edge* edge::oprev() const
{
    return rot() -> onext() -> rot();
}

// Returns next (ccw) edge around left face/vertex after current edge
edge* edge::fnext() const
{
    return invrot() -> onext() -> rot();
}

// Returns previous (cw) edge around left face/vertex before current edge
edge* edge::fprev() const
{
    return invrot() -> oprev() -> rot();
}

/* Edge assignment */

void edge::setTwin(edge* &e)
{
    int ind = quadedge::shift_up_mod4(type, 2);
    par -> setEdge(ind, e);
    e -> type = ind;
    e -> par = par;
}

void edge::setRot(edge* &e)
{
    int ind = quadedge::shift_up_mod4(type, 1);
    par -> setEdge(ind, e);
    e -> type = ind;
    e -> par = par;
}

/* Endpoint getter/setters/checks */

vertex* edge::getOrigin() const
{
    return orig;
}

vertex* edge::getDest() const
{
    return twin() -> orig;
}

// Return origin if it was set, otherwise return default vertex
vertex edge::origin() const
{
    vertex* res = getOrigin();
    return res ? *res : vertex();
}

// Return destination if it was set, otherwise return default vertex
vertex edge::destination() const
{
    vertex* res = getDest();
    return res ? *res : vertex();
}

// Sets origin to o and destination to d
// Lf and rf parameters can be used to label left face as lf and right face as rf
void edge::setEndpoints(vertex* o, vertex* d = NULL, vertex* lf = NULL, vertex* rf = NULL)
{
    orig = o;
    if (d) twin() -> orig = d;
    if (lf) invrot() -> orig = lf;
    if (rf) rot() -> orig = rf;
}

// Checks if origin and destination addresses are same
bool edge::sameEndpoints(edge* o)
{
    return getOrigin() == o -> getOrigin() and getDest() == o -> getDest();
}

// Checks if origin and destination addresses are flipped
bool edge::flippedEndpoints(edge* o)
{
    return getOrigin() == o -> getDest() and getDest() == o -> getOrigin();
}

/* Edge operations */

// Creates non-looping edge that does not split plane
// Use makeEdge() -> rot() to create loop that splits plane into two faces
edge* makeEdge()
{
    quadedge *qe = new quadedge;
    return qe -> getEdge(0);
}

// Merges origin rings of a and b and of a's dual and b's dual
// Accomplishes this by swapping values in(a -> next, b -> next) and in (dual a -> next, dual b -> next)
void splice(edge* a, edge* b)
{
    edge* dual_a = a -> onext() -> rot();
    edge* dual_b = b -> onext() -> rot();

    edge* a_next = a -> onext();
    edge* b_next = b -> onext();
    edge* dual_a_next = dual_a -> onext();
    edge* dual_b_next = dual_b -> onext();

    a -> next = b_next;
    b -> next = a_next;
    dual_a -> next = dual_b_next;
    dual_b -> next = dual_a_next;
}

// Deletes edge e after disconnecting it from its origin rings
// Returns pair where first is previous edge in origin ring of e and second is previous edge in origin ring of e's twin/reverse
std::pair <edge*, edge*> deleteEdge(edge* e)
{
    std::pair <edge*, edge*> res = {e -> oprev(), e -> twin() -> oprev()};
    splice(e, e -> oprev());
    splice(e -> twin(), e -> twin() -> oprev());
    return res;
}

// Assumes a and b represent twins
// Glues a and b together and connects their left faces with a dual edge
// Returns pointer to an arbitrary edge that still exists
edge* mergeTwins(edge* a, edge* b)
{
    std::cout<<"Merging: "<<*a<<" with "<<*b<<std::endl;
    std::cout<<"ab: "<<*a<<" and "<<*b<<std::endl;

    vertex* right_face = b -> invrot() -> getOrigin();

    a -> fnext() -> setNext(b);
    b -> fnext() -> setNext(a);

    edge* b_twin_onext = b -> twin() -> onext();
    edge* a_twin_onext = a -> twin() -> onext();
    a -> twin() -> fnext() -> setNext(b_twin_onext);
    b -> twin() -> fnext() -> setNext(a_twin_onext);

    edge* b_rot_onext = b -> rot() -> onext();
    edge* a_rot_onext = a -> rot() -> onext();

    a -> rot() -> oprev() -> setNext(b_rot_onext);
    b -> rot() -> oprev() -> setNext(a_rot_onext);

    edge* b_invrot = b -> invrot();
    a -> setTwin(b);
    a -> setRot(b_invrot);

    //a -> rot() -> setEndpoints(a -> getOrigin(), a -> getDest(), a -> invrot() -> getOrigin(), right_face);
    if (a -> twin() -> twin() != a)
    {
        std::cout<<"twin twin not a"<<std::endl;
        throw "failure";
    }
    if (a -> rot() != a -> twin() -> invrot())
    {
        std::cout<<"Rot is not twin invrot"<<std::endl;
        throw "failure";
    }

    return a;
}

#endif
