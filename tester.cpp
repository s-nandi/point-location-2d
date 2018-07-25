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
    debug(std::cout, p);
    std::cout<<std::endl;

    return 0;
}
