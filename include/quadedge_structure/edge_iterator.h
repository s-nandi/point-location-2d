#ifndef EDGE_ITERATOR_H_DEFINED
#define EDGE_ITERATOR_H_DEFINED

#include "quadedge_structure/edge.h"
#include <iterator>

class edge;

class edge::iterator : public std::forward_iterator_tag
{
private:
    edge *start, *curr;
    bool incremented;
    incidenceMode mode;
public:
    iterator(edge* e, incidenceMode im) : mode(im), incremented(false) {start = curr = e;}
    bool operator != (iterator);
    bool operator == (iterator);
    edge& operator * () {return *curr;}
    edge* operator -> () {return &(this -> iterator::operator*());}
    edge::iterator operator++ ();

    friend iterator edge::begin(incidenceMode);
    friend iterator edge::end(incidenceMode);
};

#endif
