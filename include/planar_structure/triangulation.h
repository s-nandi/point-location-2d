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
class online_point_location;
class walking_point_location;

class triangulation : public plane
{
private:
    edge* init_bounding_box(const box&);

    void fixDelaunayCondition(point, edge*);
    void addPoint(point, int, online_point_location&, triangulationType);

    void init_triangulation(std::vector <point>&, triangulationType, const box& = box{0, 0, 0, 0});
    void init_triangulation(std::vector <point>&, online_point_location&, triangulationType = delaunayTriangulation, const box& = box{0, 0, 0, 0});
public:
    int numDelaunayFlips = 0;

    void read_PT_file(std::istream &is, triangulationType = delaunayTriangulation);
    void generateRandomTriangulation(int, triangulationType = delaunayTriangulation, const box& = box{-INF, INF, INF, -INF});
    void generateRandomTriangulation(int, online_point_location&, triangulationType = delaunayTriangulation, const box& = box{-INF, INF, INF, -INF});
};

#endif
