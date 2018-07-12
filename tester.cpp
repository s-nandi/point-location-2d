#include <iostream>
#include <vector>

#include "vertex.h"
#include "plane.h"

int main()
{
    int n;
    std::cin >> n;

    std::vector <point> points(n);
    for (int i = 0; i < n; i++)
    {
        std::cin >> points[i];
    }

    int m;
    std::cin >> m;
    std::vector <std::vector<int>> triangles(m);
    for (int i = 0; i < m; i++)
    {
        int a, b, c;
        std::cin >> a >> b >> c;
        triangles[i] = {a, b, c};
    }

    plane p;
    edge* startEdge = p.init_subdivision(points, triangles);
    debug(std::cout, p);

    /*
    plane p;

    p.init_polygon({{0, 0}, {1, 0}, {2, 0}, {2, 1}, {2, 2}, {0, 2}}, 5);
    debug(std::cout, p);
    */
    //edge* startingEdge = p.init_triangle({0, 0}, {1, 1}, {0, 1}, 3);


    return 0;
}
