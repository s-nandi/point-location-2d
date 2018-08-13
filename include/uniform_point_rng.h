#ifndef UNIFORM_POINT_RNG_H_DEFINED
#define UNIFORM_POINT_RNG_H_DEFINED

#include <random>
#include <tuple>
#include "quadedge_structure/vertex.h"

class uniform_point_rng
{
private:
    T minValue[2], maxValue[2];
    T range[2];
    std::mt19937 gen;
    std::uniform_real_distribution <T> dist;
public:
    uniform_point_rng(){}
    uniform_point_rng(T, T, T, T);
    uniform_point_rng(const std::tuple <T, T, T, T>&);

    point getRandom();
    std::vector <point> getRandom(int);
};

#endif
