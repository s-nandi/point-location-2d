#ifndef TRIANGULATION_H_DEFINED
#define TRIANGULATION_H_DEFINED

#include <iostream>
#include <vector>
#include <tuple>
#include "planar_structure/plane.h"

/*
* delaunayTriangulation used to ensure that the minimum angle of the triangulation is maximized and/or ensuring that the circumcircle of every triangle is empty
* arbitraryTriangulation used when the delaunay condition (empty circumcircles) is not required
*/
enum triangulationType
{
    delaunayTriangulation,
    arbitraryTriangulation
};

class edge;
class lawson_oriented_walk;

class triangulation : public plane
{
private:
    bool bounded = false;
    static const int INF = 1231231234; // Used as default value of infinity for bounding box

    edge* init_bounding_box(T, T, T, T);

    void fixDelaunayCondition(point, edge*);
    void addPoint(point, int, lawson_oriented_walk&, triangulationType);

    void init_triangulation(std::vector <point>&, triangulationType = delaunayTriangulation);
public:
    int numDelaunayFlips = 0;

    triangulation(){}

    void read_PT_file(std::istream &is, triangulationType = delaunayTriangulation);
    void generateRandomTriangulation(int, triangulationType = delaunayTriangulation, T = -INF, T = INF, T = INF, T = -INF);
};

#endif
