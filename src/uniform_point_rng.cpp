#include "uniform_point_rng.h"
#include "quadedge_structure/vertex.h"
#include <assert.h>
#include <ctime>

uniform_point_rng::uniform_point_rng(T left, T top, T right, T bottom)
{
    assert(left <= right and bottom <= top);
    gen = std::mt19937{static_cast<long unsigned int>(time(0))};
    dist = std::uniform_real_distribution<T>(0.0, 1.0);
    minValue[0] = left, maxValue[0] = right;
    minValue[1] = bottom, maxValue[1] = top;
    for (int i = 0; i < 2; i++)
    {
        range[i] = maxValue[i] - minValue[i];
    }
}

point uniform_point_rng::getRandom()
{
    T rng[2];
    for (int i = 0; i < 2; i++)
    {
        rng[i] = range[i] * dist(gen) + minValue[i];
    }
    return point(rng[0], rng[1]);
}

std::vector <point> uniform_point_rng::getRandom(int numPoints)
{
    std::vector <point> result(numPoints);
    for (int i = 0; i < numPoints; i++)
    {
        result[i] = getRandom();
    }
    return result;
}
