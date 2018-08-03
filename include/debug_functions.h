#ifndef DEBUG_FUNCTIONS_H_DEFINED
#define DEBUG_FUNCTIONS_H_DEFINED

#include "planar_structure/plane.h"
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
            for (auto it = rot -> begin(incidentOnFace); it != rot -> end(incidentOnFace); ++it)
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
            for (auto it = rot -> begin(incidentOnFace); it != rot -> end(incidentOnFace); ++it)
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
        lawson_oriented_walk locator(pln, {rememberingWalk});
        lawson_oriented_walk rem_locator(pln, {fastRememberingWalk}, 1);
        os << "How many point location queries will you make?:" << std::endl;
        int q;
        is >> q;
        for (int i = 0; i < q; i++)
        {
            point p;
            is >> p;

            os << "For regular walk" << std::endl;
            auto e = locator.locate(p);
            if (e)
            {
                os << "Point " << p << " is in face: " << std::endl;
                for (auto it = e -> begin(incidentOnFace); it != e -> end(incidentOnFace); ++it)
                {
                    os << *it << std::endl;
                }
            }
            else
            {
                os << "Point " << p << " is out of bounds" << std::endl;
            }

            os << "For remembering walk" << std::endl;
            auto e_rem = rem_locator.locate(p);
            if (e_rem)
            {
                os << "Point " << p << " is in face: " << std::endl;
                for (auto it = e_rem -> begin(incidentOnFace); it != e_rem -> end(incidentOnFace); ++it)
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
