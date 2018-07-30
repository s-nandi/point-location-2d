#include "quadedge_structure/edge.h"
#include "quadedge_structure/edge_iterator.h"
#include <assert.h>

/* Edge Functions */

std::ostream& operator << (std::ostream &os, const edge &e)
{
    return os << e.origin() << " to " << e.destination() << " (" << e.invrot() -> origin() << ", " << e.invrot() -> destination() << ")";
}

/* Edge Iterator Functions */

// Assumes that o is an iterator with the same start and mode as this
bool edge::iterator::operator != (iterator o)
{
    assert(start == o.start and mode == o.mode);
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
    switch (mode)
    {
        case incidentToFace:
            curr = curr -> fnext();
            break;
        case incidentToOrigin:
            curr = curr -> onext();
            break;
        case incidentToDestination:
            curr = curr -> twin() -> onext() -> twin();
            break;
        case incidentToEdge:
            curr = curr -> twin();
            break;
    }
    incremented = true;
    return *this;
}

edge::iterator edge::begin(incidenceMode im)
{
    return iterator(this, im);
}

edge::iterator edge::end(incidenceMode im)
{
    auto it = iterator(this, im);
    it.incremented = true;
    return it;
}
