#include "quadedge_structure/edge.h"
#include "quadedge_structure/edge_iterator.h"
#include <assert.h>

/* Edge Functions */

std::ostream& operator << (std::ostream &os, const edge &e)
{
    return os << e.origin() << " to " << e.destination() << " (" << e.invrot() -> origin() << ", " << e.invrot() -> destination() << ")";
}

/* Edge Iterator Functions */

edge::iterator::iterator(edge* e, incidenceMode im, bool rev) : incremented(false), mode(im), reversed(rev)
{
    start = curr = e;
}

// Assumes that o is an iterator with the same start and mode as this
bool edge::iterator::operator != (iterator o)
{
    assert(start == o.start and mode == o.mode and reversed == o.reversed);
    return incremented != o.incremented or curr != o.curr;
}

bool edge::iterator::operator == (iterator o)
{
    return !(*this != o);
}

// Assumes that iterator has not reached the end of its cycle
edge::iterator edge::iterator::operator++ ()
{
    assert(!incremented or curr != start);
    if (!reversed) // go to next for begin() and end()
    {
        switch (mode)
        {
            case incidentOnFace:
                curr = curr -> fnext();
                break;
            case incidentToOrigin:
                curr = curr -> onext();
                break;
        }
    }
    else // go to prev for rbegin() and rend()
    {
        switch (mode)
        {
            case incidentOnFace:
                curr = curr -> fprev();
                break;
            case incidentToOrigin:
                curr = curr -> oprev();
                break;
        }
    }
    incremented = true;
    return *this;
}

edge::iterator edge::begin(incidenceMode im)
{
    return iterator(this, im, false);
}

edge::iterator edge::end(incidenceMode im)
{
    auto it = iterator(this, im, false);
    it.incremented = true;
    return it;
}

edge::iterator edge::rbegin(incidenceMode im)
{
    return iterator(this, im, true);
}

edge::iterator edge::rend(incidenceMode im)
{
    auto it = iterator(this, im, true);
    it.incremented = true;
    return it;
}
