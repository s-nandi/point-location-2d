#ifndef DEBUG_H_DEFINED
#define DEBUG_H_DEFINED

#include "quad_edge/plane.h"
#include "point_location/walking/lawson_oriented_walk.h"

/* Debugging planar subdivision information */

struct debug
{
    static void loud_print(std::ostream &os, plane &p)
    {
        auto faces = p.traverse(dualGraph, traverseEdges);
        os << "Face Traversal (all edges) " << faces.size() << std::endl;
        for (edge *f: faces)
        {
            os << "Face: "<< std::endl;
            os <<*f<< std::endl;
            os << "Around: "<< std::endl;
            edge* rot = f -> rot();
            for (auto it = rot -> begin(incidentToFace); it != rot -> end(incidentToFace); ++it)
            {
                os << *it << std::endl;
            }
        }
        auto vertices = p.traverse(primalGraph, traverseNodes);
        os << "Vertex Traversal (all nodes)" << std::endl;
        for (edge *v: vertices)
        {
            os << "Vertex: "<< std::endl;
            os << v -> origin()<< " | prev: " << v -> destination() << std::endl;
            os << "Around: " << std::endl;
            edge* rot = v -> rot();
            for (auto it = rot -> begin(incidentToFace); it != rot -> end(incidentToFace); ++it)
            {
                os << *it << std::endl;
            }
        }
        os << "Going around first face" << std::endl;
        for (edge &e: *faces[0])
        {
            for (edge &e2: e)
                os << e2 << std::endl;
        }
    }

    static void quiet_print(std::ostream &os, plane &p)
    {
        auto faces = p.traverse(dualGraph, traverseEdges);
        os << "Face Traversal (all edges)" << std::endl;
        for (edge *f: faces)
        {
            os << *f << '\n';
        }
        auto vertices = p.traverse(primalGraph, traverseNodes);
        os << "Vertex Traversal (all nodes)" << std::endl;
        for (edge *v: vertices)
        {
            os << v -> origin() << " | prev: " << v -> destination() << '\n';
        }
    }

    static void check_lawson(std::istream &is, std::ostream &os, plane &pln)
    {
        lawson_oriented_walk locator(pln);
        lawson_oriented_walk rem_locator(pln, false, true);
        os << "How many point location queries will you make?:" << std::endl;
        int q;
        is >> q;
        for (int i = 0; i < q; i++)
        {
            point p;
            is >> p;

            auto e = locator.locate(p);
            auto e_rem = rem_locator.locate(p);
            if (e)
            {
                if (e -> leftface().getLabel() != e_rem -> leftface().getLabel())
                {
                    throw "Different face results";
                }
                os << "Point " << p << " is in face: " << std::endl;
                for (auto it = e -> begin(incidentToFace); it != e -> end(incidentToFace); ++it)
                {
                    os << *it << std::endl;
                }
            }
            else
            {
                os << "Point " << p << " is out of bounds" << std::endl;
            }
        }
        os << "For non-remembering" << std::endl;
        os << "Orientation tests: "<< locator.numTests << std::endl;
        os << "Faces Traversed: "<< locator.numFaces << std::endl;

        os << "For remembering" << std::endl;
        os << "Orientation tests: "<< rem_locator.numTests << std::endl;
        os << "Faces Traversed: "<< rem_locator.numFaces << std::endl;
    }
};

#endif
