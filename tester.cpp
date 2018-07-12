#include <iostream>
#include <vector>

#include "point.h"
#include "quadedge.h"

int main()
{
    int n;
    std::cin>>n;

    std::vector <point> points(n);
    for (int i = 0; i < n; i++)
    {
        std::cin>>points[i];
    }

    return 0;
}
