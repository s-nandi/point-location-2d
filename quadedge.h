#ifndef QUADEDGE_H_DEFINED
#define QUADEDGE_H_DEFINED

#include "edge.h"

class plane;

class quadedge
{
friend edge;
friend plane;
private:
    edge* e[4];
    int lastUsed = -1;

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
        e[i] -> type = i;
        e[i] -> par = this;
    }
    e[0] -> setNext(e[0]);
    e[1] -> setNext(e[3]);
    e[2] -> setNext(e[2]);
    e[3] -> setNext(e[1]);
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
inline int edge::shift_up_mod4(int start, int shift)
{
    return (start + shift < 4) ? start + shift : start + shift - 4;
}

// Assumes start is valid mod 4 and (start - shift) wraps around 0 at most once
inline int edge::shift_down_mod4(int start, int shift)
{
    return (start - shift >= 0) ? start - shift : start - shift + 4;
}

/* Edge algebra */

// Returns dual edge pointing from right face/vertex towards left face/vertex (rotated ccw)
edge* edge::rot() const
{
    return getParent() -> getEdge(shift_up_mod4(type, 1));
}

// Returns dual edge pointing from left face/vertex towards right face/vertex (rotated cw)
edge* edge::invrot() const
{
    return getParent() -> getEdge(shift_down_mod4(type, 1));
}

// Returns flipped edge starting at destination and ending at origin
edge* edge::twin() const
{
    return getParent() -> getEdge(shift_up_mod4(type, 2));
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
    int ind = shift_up_mod4(type, 2);
    par -> setEdge(ind, e);
    e -> type = ind;
    e -> par = par;
}

/* Endpoint getter/setters */

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

// Return left face if it was set, otherwise return default vertex
vertex edge::leftface() const
{
    vertex* res = invrot() -> getOrigin();
    return res ? *res : vertex();
}

// Return right face if it was set, otherwise return default vertex
vertex edge::rightface() const
{
    vertex* res = invrot() -> getDest();
    return res ? *res : vertex();
}

// Sets origin/destination to o and d respectively
// Lf and rf parameters can be used to label left face as lf and right face as rf
// NULL can be passed for any of the parameters to prevent setting specific labels
void edge::setEndpoints(vertex* o = NULL, vertex* d = NULL, vertex* lf = NULL, vertex* rf = NULL)
{
    if (o) orig = o;
    if (d) twin() -> orig = d;
    if (lf) invrot() -> orig = lf;
    if (rf) rot() -> orig = rf;
}

/* Edge operations */

// Creates non-looping edge whose left and right faces are the same
// Use makeEdge() -> rot() to create loop that splits plane into two faces
edge* makeEdge()
{
    quadedge *qe = new quadedge;
    return qe -> getEdge(0);
}

// Merges origin rings of a and b and of a's dual and b's dual
// Swaps values in (a -> next, b -> next) and in (dual a -> next, dual b -> next)
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

/*
* Assumes b is not the next edge on the left face of a and that planarity is preserved
* Also assumes that face_number is non-negative if passed in
* Connects destination of a to origin of b and sets endpoints/faces of the created edge
* Optional face_number parameter used to label the new face created (to the left of the new edge)
*/
edge* connect(edge* a, edge* b, int face_number = -1)
{
    assert(a -> getDest() != b -> getOrigin());
	edge* e = makeEdge();
	splice(e, a -> fnext());
	splice(e -> twin(), b);
    e -> setEndpoints(a -> getDest(), b -> getOrigin(), NULL, a -> invrot() -> getOrigin());

    // If face_number is not given, set the left face of the new edge to be the same as its right face
    // Can be used to split a face into smaller faces while keeping the same face labels (ex. triangulating a polygon)
	if (face_number == -1)
    {
        e -> invrot() -> setEndpoints(a -> invrot() -> getOrigin());
    }
    // If face_number is given, set the origin for all dual edges originating from the left face of e to a new face
    else
    {
        vertex* new_face = new vertex(face_number);
        edge* irot = e -> invrot();
        for (auto it = irot -> begin(incidentToOrigin); it != irot -> end(incidentToOrigin); ++it)
        {
            it -> setEndpoints(new_face);
        }
    }
	return e;
}

// Deletes edge e after disconnecting it from its origin rings
// Sets the left face of every edge on the same as e, to the right face of e
// Effectively removes the left face of e
void deleteEdge(edge* e)
{
    vertex* right_face = e -> rot() -> getOrigin();
    for (auto it = e -> begin(incidentToFace); it != e -> end(incidentToFace); ++it)
    {
        it -> invrot() -> setEndpoints(right_face);
    }
    splice(e, e -> oprev());
    splice(e -> twin(), e -> twin() -> oprev());

}

// Assumes a and b represent twins
// Glues a and b together and connects their left faces
// Returns pointer to an arbitrary edge that still exists
edge* mergeTwins(edge* a, edge* b)
{
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
    a -> invrot() -> setTwin(b_invrot);

    return a;
}

#endif
