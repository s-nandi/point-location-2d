#include <iostream>
#include <vector>

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
    auto faces = p.traverseFaces(traversalMode::useVertexOnce);
    std::cout<<"Face Traversal"<<std::endl;
    for (edge* f: faces)
    {
        std::cout<<*f<<'\n';
    }
    auto vertices = p.traverseVertices(traversalMode::useVertexOnce);
    std::cout<<"Vertex Traversal"<<std::endl;
    for (edge* v: vertices)
    {
        std::cout<<v -> origin()<<" | prev: " << v -> destination() << '\n';
    }

    return 0;
}
