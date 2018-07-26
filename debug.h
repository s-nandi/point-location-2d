#ifndef DEBUG_H_DEFINED
#define DEBUG_H_DEFINED

#include <cctype>
#include "plane.h"

/* Debugging planar subdivision information */

void debug(std::ostream &os, plane &p)
{
    os << "Time before facetraversal: " << p.time << std::endl;
    auto faces = p.traverseFaces(traversalMode::useEdgeOnce);
    os << "Face Traversal " << faces.size() << std::endl;
    for (edge* f: faces)
    {
        os << "Face: "<< std::endl;
        os <<*f<< std::endl;
        auto around = incidentToFace(f -> rot());
        os << "Around: "<< " "<< around.size() << std::endl;
        for (edge* ar: around)
        {
            os << *ar << std::endl;
        }
    }
    os << "Time before vertextraversal: " << p.time << std::endl;
    auto vertices = p.traverseVertices(traversalMode::useVertexOnce);
    os << "Vertex Traversal" << std::endl;
    for (edge* v: vertices)
    {
        os << "Vertex: "<< std::endl;
        os << v -> origin()<< " | prev: " << v -> destination() << std::endl;
        auto around = incidentToFace(v -> rot());
        os << "Around: " << std::endl;
        for (edge* ar: around)
        {
            os << *ar << std::endl;
        }
    }
}

void debug_quiet(std::ostream &os, plane &p)
{
    auto faces = p.traverseFaces(traversalMode::useEdgeOnce);
    os << "Face Traversal" << std::endl;
    for (edge* f: faces)
    {
        os << *f << '\n';
    }
    auto vertices = p.traverseVertices(traversalMode::useVertexOnce);
    os << "Vertex Traversal" << std::endl;
    for (edge* v: vertices)
    {
        os << v -> origin() << " | prev: " << v -> destination() << '\n';
    }
}

void debug_tour(std::ostream &os, plane &p)
{
    os << "Start tour" << std::endl;
    edge* curr = p.incidentEdge;
    while(true)
    {
        os << "Curr edge: " << *curr << std::endl;
        char command;
        std::cin >> command;
        command = std::toupper(command);
        if (command == 'T')
        {
            curr = curr -> twin();
        }
        else if(command == 'R')
        {
            curr = curr -> rot();
        }
        else if(command == 'I')
        {
            curr = curr -> invrot();
        }
        else if(command == 'F')
        {
            curr = curr -> fnext();
        }
        else if(command == 'B')
        {
            curr = curr -> fprev();
        }
        else if(command == 'O')
        {
            curr = curr -> onext();
        }
        else if(command == 'P')
        {
            curr = curr -> oprev();
        }
        else if(command == 'S')
        {
            curr = p.incidentEdge;
        }
        else if(command == 'E')
        {
            break;
        }
        else if(command == 'C')
        {
            for (int i = 0; i < 300; i++)
            {
                os << std::endl;
            }
        }
        else
        {
            os << "Invalid command: " << command <<std::endl;
        }
    }
}

#endif
