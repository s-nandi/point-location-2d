#include "quadedge_structure/quadedge.h"
#include <assert.h>

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

/* Endpoint getter/setter */

vertex* edge::getOrigin() const
{
    return orig;
}

vertex* edge::getDest() const
{
    return twin() -> orig;
}

vertex& edge::origin() const
{
    return *getOrigin();
}

vertex& edge::destination() const
{
    return *getDest();
}

vertex& edge::leftface() const
{
    return *(invrot() -> getOrigin());
}

vertex& edge::rightface() const
{
    return *(invrot() -> getDest());
}

point edge::originPosition() const
{
    return origin().getPosition();
}

point edge::destinationPosition() const
{
    return destination().getPosition();
}

int edge::leftfaceLabel() const
{
    return leftface().getLabel();
}

int edge::rightfaceLabel() const
{
    return rightface().getLabel();
}

// Sets origin/destination to o and d respectively
// Lf and rf parameters can be used to label left face as lf and right face as rf
// NULL can be passed for any of the parameters to prevent setting specific labels
void edge::setEndpoints(vertex* o, vertex* d, vertex* lf, vertex* rf)
{
    if (o) orig = o;
    if (d) twin() -> orig = d;
    if (lf) invrot() -> orig = lf;
    if (rf) rot() -> orig = rf;
}

// Given an edge e, relabels left face pointers for all edges on same left face as e to parameter f
void edge::labelFace(vertex* f)
{
    edge* irot = this -> invrot();
    for (auto it = irot -> begin(incidentToOrigin); it != irot -> end(incidentToOrigin); ++it)
        it -> setEndpoints(f);
}

/* Edge Operations */

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
* Assumes a's destination is not b's origin and that face_number is strictly positive
* Connects destination of a to origin of b and sets endpoints/faces of the created edge
* face_number parameter used to label the new face created (to the left of the new edge)
* If face_number is -1, the new face created is labeled the same as the left face of a
*/
edge* connect(edge* a, edge* b, int face_number)
{
    // Make sure edge a does not already connect into edge b
    assert(a -> getDest() != b -> getOrigin());
	edge* e = makeEdge();
	splice(e, a -> fnext());
	splice(e -> twin(), b);
    e -> setEndpoints(a -> getDest(), b -> getOrigin(), a -> invrot() -> getOrigin(), a -> invrot() -> getOrigin());
    // Sets the left face pointer of all edges on the same left face as e to a new face
    // If face_number not passed in, uses the preexisting face label (left face of a)
    vertex* new_face;
    if (face_number != -1)
        new_face = new vertex(face_number);
    else
        new_face = new vertex(e -> leftface().getLabel());
    e -> labelFace(new_face);
	return e;
}

// Deletes edge e after disconnecting it from its origin rings
// Sets the left face of every edge on the same as e, to the right face of e
// Effectively removes the left face of e
void deleteEdge(edge* e)
{
    vertex* right_face = e -> rot() -> getOrigin();
    for (auto it = e -> begin(incidentOnFace); it != e -> end(incidentOnFace); ++it)
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

// Assumes that e not a boundary edge (ie. does not have the extremeVertex as its right or left face)
// Rotates e ccw in its enclosing polygon (the union of the edges on its left and right faces excluding itself)
// Endpoint/face pointers adjusted to ensure that left/right faces of the rotated edge match the left/right faces of the previous edge e
// Returns pointer to the newly rotated edge e
edge* rotateInEnclosing(edge* e)
{
    vertex* left_face = e -> invrot() -> getOrigin();
    vertex* right_face = e -> rot() -> getOrigin();
    // Make sure that e is not a boundary edge
    assert(left_face -> getLabel() != 0 and right_face -> getLabel() != 0);
    edge* a = e -> oprev();
    edge* b = e -> twin() -> oprev();
    // Disconnect e from the enclosing polygon
    splice(e, a);
    splice(e -> twin(), b);
    // Connect e to the next edges on a's and b's left faces (note that edges in a face are oriented ccw)
    splice(e, a -> fnext());
    splice(e -> twin(), b -> fnext());
    e -> setEndpoints(a -> getDest(), b -> getDest(), left_face, right_face);
    a -> invrot() -> setEndpoints(left_face);
    b -> invrot() -> setEndpoints(right_face);
    return e;
}

