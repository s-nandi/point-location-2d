#include "quadedge_structure/vertex.h"

// Thrown if label for vertex is negative
struct invalidLabelException : std::exception
{
    const char * what () const throw ()
    {
    	return "Invalid Label for Vertex: Labels Must Be Strictly Positive";
    }
};

vertex::vertex(int i) : lastUsed(-1)
{
    if (i < 0) throw invalidLabelException();
    label = i;
}

vertex::vertex(point p, int i) : lastUsed(-1)
{
    if (i < 0) throw invalidLabelException();
    position = p, label = i;
    hasPosition = true;
}

// Returns false if vertex was used during or after given timestamp
// Otherwise returns true and sets lastUsed to timestamp
// Used to ensure that each vertex is only used once in traversal
bool vertex::use(int timestamp)
{
    if (timestamp <= lastUsed) return false;
    else
    {
        lastUsed = timestamp;
        return true;
    }
}

std::ostream& operator << (std::ostream &os, const vertex &v)
{
    if (v.hasPosition) return os << "[" << v.position << " : " << v.label << "]";
    else return os << v.label; // -1 labels are ignored (non-zero label must be
}
