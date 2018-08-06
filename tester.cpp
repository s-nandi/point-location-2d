#include <iostream>
#include <assert.h>
#include <chrono>
#include <cmath>
#include <algorithm>
#include "uniform_point_rng.h"
#include "planar_structure/triangulation.h"
#include "quadedge_structure/quadedge.h"
#include "point_location/walking/lawson_oriented_walk.h"

/* Helper Functions for point inclusion in face and delaunay condition checking */
bool in_padded_bounding_box(point p, int left, int top, int right, int bottom)
{
    return p.x >= left - 1 and p.x <= right + 1 and p.y >= bottom - 1 and p.y <= top + 1;
}

bool in_face(point p, edge* e)
{
    // Make sure that edge is not on the outside face
    assert(e -> leftface().getLabel() != 0);
    for (edge &face_edge: *e)
    {
        // Assumes that edges in a face are ccw oriented
        // If the point is strictly to the right of an edge, it cannot be inside the face
        if (orientation(e -> origin().getPosition(), e -> destination().getPosition(), p) > 0)
        {
            return false;
        }
    }
    return true;
}

bool fulfills_delaunay(edge *e)
{
    if (e -> leftface().getLabel() == 0 or e -> rightface().getLabel() == 0)
        return true;
    point a = e -> origin().getPosition();
    point b = e -> destination().getPosition();
    point c = e -> fnext() -> destination().getPosition();
    point d = e -> twin() -> fnext() -> destination().getPosition();
    assert(orientation(a, b, c) <= 0);
    assert(orientation(b, a, d) <= 0);
    bool delaunay_abc = inCircle(d, a, b, c) <= 0;
    bool delaunay_adb = inCircle(c, b, a, d) <= 0;
    bool res = delaunay_abc and delaunay_adb;
    if (!res)
    {
        std::cout << "Failed delaunay: " << std::endl;
        std::cout << "Points: " << a << " " << b << " " << c << " " << d << std::endl;
    }
    return res;
}

/* Helper Functions for Timing */

using clock_type = std::chrono::high_resolution_clock;
using duration_type =  std::chrono::duration<double>;
using time_type = std::chrono::time_point<clock_type>;

clock_type timer;
time_type start_time, end_time;

inline void startTimer()
{
    start_time = clock_type::now();
}

inline double endTimer()
{
    end_time = clock_type::now();
    duration_type duration = end_time - start_time;
    return duration.count();
}

/* Helper Functions for Printing Timer and Correctness*/

void print_time(const std::string &name, double t)
{
    std::cout << "Time taken for " << name << ": " << t << " s" << std::endl;
}

void print_time(const std::string &name)
{
    std::cout << "Time taken for " << name << ": " << ((duration_type) (end_time - start_time)).count() << " s" << std::endl;
}

void print_percent_correct(const std::string &name, int correct, int total)
{
    double percentage = (double) (correct * 100) / total;
    std::cout << name << ": " << correct << " / " << total << " (" << percentage << "%)" << std::endl;
}

/* Tests */

void test_random_point_location_in_random_triangulation(int numPoints = 1000, bool delaunay = false)
{
    triangulation tr = triangulation();
    int numCorrect = 0;

    int left = -1000000, top = 10000000, right = 500000, bottom = -10000000;
    double padding_coeff = 1.3; // So that some points are outside bounding box;

    startTimer();
    if (!delaunay)
    {
        tr.generateRandomTriangulation(numPoints, arbitraryTriangulation, left, top, right, bottom);
        endTimer();
        print_time("Generating arbitrary random triangulation");
    }
    else
    {
        tr.generateRandomTriangulation(numPoints, delaunayTriangulation, left, top, right, bottom);
        endTimer();
        print_time("Generating delaunay random triangulation");
    }

    std::vector <int> faces;
    for (edge* e: tr.traverse(dualGraph, traverseNodes))
    {
        faces.push_back(e -> origin().getLabel());
    }
    std::sort(faces.begin(), faces.end());
    for (int i = 0; i < faces.size(); i++)
    {
        assert(i == faces[i]);
    }

    uniform_point_rng rng(padding_coeff * left, padding_coeff * top, padding_coeff * right, padding_coeff * bottom);
    std::vector <point> locating = rng.getRandom(numPoints);

    lawson_oriented_walk locator(tr, {stochasticWalk, sampleStart, fastRememberingWalk}, std::pow(numPoints, 1.0/4.0), std::pow(numPoints, 1.0/3.0));
    for (edge* e: tr.traverse(primalGraph, traverseEdges))
    {
        locator.addEdge(e);
    }
    startTimer();
    for (point p: locating)
    {
        edge* e = locator.locate(p);
        bool expected_in_box = in_padded_bounding_box(p, left, top, right, bottom);
        bool found_in_box = !(e == NULL);
        if (expected_in_box != found_in_box)
        {
            std::cout << "Incorrect: " << p << " in plane was supposed to be " << expected_in_box << " but you found " << found_in_box << std::endl;
        }
        else if(found_in_box and !in_face(p, e))
        {
            std::cout << "Incorrect: " << p << " is not in face: " << std::endl;
            for (edge &face_edges: *e)
            {
                std::cout << face_edges << std::endl;
            }
        }
        else
        {
            numCorrect++;
        }
    }
    endTimer();
    if (!delaunay)
        print_percent_correct("test_random_point_location_in_random_arbitrary_triangulation: ", numCorrect, numPoints);
    else
        print_percent_correct("test_random_point_location_in_random_delaunay_triangulation: ", numCorrect, numPoints);
}

void test_delaunay_condition_for_random_triangulation(int numPoints = 100000)
{
    int numCorrect = 0, total = 0;
    triangulation tr = triangulation();
    startTimer();
    tr.generateRandomTriangulation(numPoints, delaunayTriangulation);
    endTimer();
    // Total number of flips is bounded by 9 * n + 1
    assert(tr.numDelaunayFlips <= 9 * numPoints + 1);
    for (edge* e: tr.traverse(primalGraph, traverseEdges))
    {
        total++;
        if (fulfills_delaunay(e))
            numCorrect++;
    }
    print_percent_correct("test_delaunay_condition_for_random_triangulation", numCorrect, total);
}

void test_rng_distribution()
{
    int numPoints = 50000000;
    startTimer();
    int left = -99, top = 51, right = 101, bottom = -49;

    uniform_point_rng rng(left, top, right, bottom);

    int buckets = 20;
    int xtenths[buckets];
    int yfifths[buckets];
    for (int i = 0; i < buckets; i++)
    {
        xtenths[i] = yfifths[i] = 0;
    }

    for (int i = 0; i < numPoints; i++)
    {
        point p = rng.getRandom();
        int xbucket = (int) (p.x - left) / 10;
        int ybucket = (int) (p.y - bottom) / 5;
        if(xbucket >= 0 and xbucket < buckets and ybucket >= 0 and ybucket < buckets)
        {
            xtenths[xbucket]++;
            yfifths[ybucket]++;
        }
    }
    int minx, maxx, miny, maxy;
    for (int i = 0; i < buckets; i++)
    {
        std::cout<<"i : " << i << " Xtenths: " << xtenths[i] << " Yfifths: " << yfifths[i] << std::endl;
        assert(xtenths[i] > 0 and yfifths[i] > 0);
        if (i == 0)
        {
            minx = maxx = xtenths[i];
            miny = maxy = yfifths[i];
        }
        else
        {
            miny = std::min(miny, yfifths[i]);
            maxy = std::max(maxy, yfifths[i]);
            minx = std::min(minx, xtenths[i]);
            maxx = std::max(maxx, xtenths[i]);
        }
    }
    double threshold = 1.01;
    double x_rat = (double) maxx / minx;
    double y_rat = (double) maxy / miny;
    int passed = 2;
    if (x_rat > threshold)
    {
        std::cout << "Some X values show up too often compared to others" << std::endl;
        --passed;
    }
    if (y_rat > threshold)
    {
        std::cout << "Some Y values show up too often compared to others" << std::endl;
        --passed;
    }
    print_percent_correct("test_rng_distribution", passed, 2);
    endTimer();
}

int main()
{
    /*
    test_rng_distribution();
    print_time("test_rng_distribution");
    */

    test_random_point_location_in_random_triangulation(100000);
    print_time("test_random_point_location_in_random_arbitrary_triangulation");

    test_random_point_location_in_random_triangulation(100000, true);
    print_time("test_random_point_location_in_random_delaunay_triangulation");

    test_delaunay_condition_for_random_triangulation(10000);
    print_time("test_delaunay_condition_for_random_triangulation");

    test_delaunay_condition_for_random_triangulation(20000);
    print_time("test_delaunay_condition_for_random_triangulation");

    test_delaunay_condition_for_random_triangulation(50000);
    print_time("test_delaunay_condition_for_random_triangulation");

    test_delaunay_condition_for_random_triangulation(100000);
    print_time("test_delaunay_condition_for_random_triangulation");

    return 0;
}
