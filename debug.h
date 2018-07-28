#ifndef DEBUG_H_DEFINED
#define DEBUG_H_DEFINED

#include "plane.h"

/* Debugging planar subdivision information */

struct debug
{
    static void loud(std::ostream &os, plane &p)
    {
        auto faces = p.traverse(dualGraph, traverseEdges);
        os << "Face Traversal (all edges) " << faces.size() << std::endl;
        for (edge f: faces)
        {
            os << "Face: "<< std::endl;
            os <<f<< std::endl;
            os << "Around: "<< std::endl;
            edge* rot = f.rot();
            for (auto it = rot -> begin(incidentToFace); it != rot -> end(incidentToFace); ++it)
            {
                os << *it << std::endl;
            }
        }
        auto vertices = p.traverse(primalGraph, traverseNodes);
        os << "Vertex Traversal (all nodes)" << std::endl;
        for (edge v: vertices)
        {
            os << "Vertex: "<< std::endl;
            os << v.origin()<< " | prev: " << v.destination() << std::endl;
            os << "Around: " << std::endl;
            edge* rot = v.rot();
            for (auto it = rot -> begin(incidentToFace); it != rot -> end(incidentToFace); ++it)
            {
                os << *it << std::endl;
            }
        }
    }

    static void quiet(std::ostream &os, plane &p)
    {
        auto faces = p.traverse(dualGraph, traverseEdges);
        os << "Face Traversal (all edges)" << std::endl;
        for (edge f: faces)
        {
            os << f << '\n';
        }
        auto vertices = p.traverse(primalGraph, traverseNodes);
        os << "Vertex Traversal (all nodes)" << std::endl;
        for (edge v: vertices)
        {
            os << v.origin() << " | prev: " << v.destination() << '\n';
        }
    }
};

#endif
