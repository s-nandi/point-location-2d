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
    edge e[4];

    quadedge();
    edge* getEdge(int i) {return &e[i];}
    friend edge* makeEdge();

    bool use(int);
};

quadedge::quadedge()
{
    e[0].type = 0, e[0].setNext(e[0]);
    e[1].type = 1, e[1].setNext(e[3]);
    e[2].type = 2, e[2].setNext(e[2]);
    e[3].type = 3, e[3].setNext(e[1]);
    e[0].par = e[1].par = e[2].par = e[3].par = this;
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

void edge::setEndpoints(vertex* o, vertex *d)
{
    orig = o;
    twin() -> orig = d;
}

// Sets endpoints and labels left face of edge as lf, and right face as the extreme vertex
void edge::setEndpoints(vertex* o, vertex *d, vertex* lf)
{
    orig = o;
    twin() -> orig = d;
    invrot() -> orig = lf;
    rot() -> orig = &extremeVertex;
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
    delete e -> getParent();
    return res;
}

// Assumes a and b belong to the same face
// Returns edge e that connects a's origin and b's origin
// Optional left_face and right_face parameters can be used to set face labels for e
edge* splitFace(edge* a, edge* b, vertex* left_face = NULL, vertex* right_face = NULL)
{
    /*
	e = e ? e : makeEdge();
	splice(e, a -> fnext());
	splice(e -> twin(), b);
	e -> setEndpoints(a -> getDest(), b -> getOrigin());
	e -> invrot() -> setEndpoints(left_face, right_face);
	return e;
	*/
	edge* e = makeEdge();
	splice(b, e -> twin());
	splice(a, e);
	e -> setEndpoints(a -> getOrigin(), b -> getOrigin());
	e -> invrot() -> setEndpoints(left_face, right_face);
	return e;
}

// REMOVE THIS LATER
std::vector <edge*> incidentToFac(edge* edge_on_face)
{
    std::vector <edge*> edges;
    edge* startingEdge = edge_on_face;
    edge* currEdge = startingEdge;
    do
    {
        edges.push_back(currEdge);
        currEdge = currEdge -> fnext();
    }
    while (currEdge != startingEdge);
    return edges;
}

std::vector <edge*> incidentToOr(edge* edge_from_origin)
{
    std::vector <edge*> edges;
    edge* startingEdge = edge_from_origin;
    edge* currEdge = startingEdge;
    do
    {
        edges.push_back(currEdge);
        currEdge = currEdge -> onext();
    }
    while (currEdge != startingEdge);
    return edges;
}

// Assumes a and b represent twins
// Glues a and b together and connects their left faces with a dual edge
// Returns pointer to an edge that still exists
edge* mergeDuplicate(edge* &a, edge* &b)
{
    /*
    edge* prev = a -> oprev();
    edge* twin_prev = a -> twin() -> oprev();
    edge* rot_prev = a -> rot() -> oprev();
    edge* invrot_prev = a -> invrot() -> oprev();

    vertex* a_face_endpoint = a -> invrot() -> getOrigin();
    vertex* b_face_endpoint = b -> invrot() -> getOrigin();

    deleteEdge(a);
    splice(prev, b -> twin());
    splice(twin_prev, b);
    //splice(invrot_prev, b -> rot());
    //splice(rot_prev, b -> invrot());

    b -> invrot() -> setEndpoints(b_face_endpoint, a_face_endpoint);
    */

    std::cout<<"Merging: "<<*a<<std::endl;

    vertex* a_face = a -> invrot() -> getOrigin();
    vertex* b_face = b -> invrot() -> getOrigin();
    auto old_a = deleteEdge(a);
    auto old_b = deleteEdge(b);

    std::cout<<"Old a: "<<std::endl;
    std::cout<<*(old_a.first)<<" and "<<*(old_a.second)<<std::endl;
    std::cout<<"Old b: "<<std::endl;
    std::cout<<*(old_b.first)<<" and "<<*(old_b.second)<<std::endl;

    splice(old_a.first, old_b.second);
    splice(old_a.second, old_b.first);

    /*
    std::cout<<"After splice: "<<std::endl;
    auto deb = incidentToFac(old_a.first);
    for (auto ed: deb)
    {
        std::cout<<"Edge: "<<*ed<<std::endl;
        auto neigh = incidentToOr(ed);
        std::cout<<"Neighbor: "<<std::endl;
        for (auto ed2: neigh)
        {
            std::cout<<*ed2<<std::endl;
        }
    }
    std::cout<<"DONE DEBUG Splice Normal"<<std::endl;

    auto deb2 = incidentToFac(old_a.first -> invrot());
    for (auto ed: deb2)
    {
        std::cout<<"Edge: "<<*ed<<std::endl;
        auto neigh = incidentToOr(ed);
        std::cout<<"Neighbor: "<<std::endl;
        for (auto ed2: neigh)
        {
            std::cout<<*ed2<<std::endl;
        }
    }

    std::cout<<"DONE DEBUG Splice Invrot"<<std::endl;
    */

    edge* e = splitFace(old_b.second, old_a.second, a_face, b_face);

    /*
    std::cout<<"After Connect: "<<std::endl;
    auto deb3 = incidentToFac(old_a.first);
    for (auto ed: deb3)
    {
        std::cout<<"Edge: "<<*ed<<std::endl;
        auto neigh = incidentToOr(ed);
        std::cout<<"Neighbor: "<<std::endl;
        for (auto ed2: neigh)
        {
            std::cout<<*ed2<<std::endl;
        }
    }
    std::cout<<"DONE DEBUG Connect Normal"<<std::endl;

    auto deb4 = incidentToFac(old_a.first -> invrot());
    for (auto ed: deb4)
    {
        std::cout<<"Edge: "<<*ed<<std::endl;
        auto neigh = incidentToOr(ed);
        std::cout<<"Neighbor: "<<std::endl;
        for (auto ed2: neigh)
        {
            std::cout<<*ed2<<std::endl;
        }
    }

    std::cout<<"DONE DEBUG Connect Invrot"<<std::endl;
    */

    return old_a.first;
}

#endif
