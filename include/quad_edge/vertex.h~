#ifndef VERTEX_H_DEFINED
#define VERTEX_H_DEFINED

#include "../geo_primitives/point2D.h"
#include "../geo_primitives/point3D.h"

typedef point2D point;

// Thrown if label for vertex is negative
struct invalidLabelException : std::exception
{
    const char * what () const throw ()
    {
    	return "Invalid Label for Vertex: Labels Must Be Strictly Positive";
    }
};

class plane;

class vertex
{
friend plane;
private:
    int label;
    point position;
    bool hasPosition = false;
    int lastUsed = -1;

    bool use(int);
public:
    vertex(){}
    vertex(int);
    vertex(point, int);

    int getLabel() {return label;}
    point getPosition() {return position;}

    friend std::ostream& operator << (std::ostream&, const vertex&);
} extremeVertex = vertex(0);

vertex::vertex(int i)
{
    if (i < 0) throw invalidLabelException();
    label = i;
}

vertex::vertex(point p, int i)
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

#endif
