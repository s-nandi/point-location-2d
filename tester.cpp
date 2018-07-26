#include <iostream>
#include <vector>

#include "plane.h"
#include "debug.h"

int main()
{
    int numPoints, numFaces, numEdges;
    std::cin >> numPoints >> numFaces >> numEdges;

    std::vector <point> points(numPoints);
    for (int i = 0; i < numPoints; i++)
    {
        std::cin >> points[i];
    }
    std::vector <std::vector<int>> triangles(numFaces);
    for (int i = 0; i < numFaces; i++)
    {
        int sz;
        std::cin >> sz;
        for (int j = 0; j < sz; j++)
        {
            int ind;
            std::cin >> ind;
            triangles[i].push_back(ind);
        }
    }

    plane p;
    p.init_subdivision(points, triangles);
    debug_tour(std::cout, p);

    return 0;
}
