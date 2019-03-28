#include <iostream>
#include <fstream>
#include <cassert>
#include <cmath>
#include <algorithm>
#include <memory>
#include "planar_structure/triangulation.h"
#include "point_location/walking/lawson_oriented_walk.h"
#include "point_location/walking/walking_point_location.h"
#include "point_location/non_walking/slab_decomposition.h"
#include "point_location/non_walking/naive_quadtree.h"
#include "uniform_point_rng.h"
#include "testing.h"

#include "data_structures/quadtree.h"

/* Helper Functions for point inclusion in face and delaunay condition checking */
bool in_padded_bounding_box(point p, int left, int top, int right, int bottom)
{
    return p.x >= left - 1 and p.x <= right + 1 and p.y >= bottom - 1 and p.y <= top + 1;
}

bool in_face(point p, edge* e)
{
    // Make sure that edge is not on the outside face
    assert(e -> leftfaceLabel() != 0);
    for (edge &face_edge: *e)
    {
        // Assumes that edges in a face are ccw oriented
        // If the point is strictly to the right of an edge, it cannot be inside the face
        if (orientation(e -> originPosition(), e -> destinationPosition(), p) > 0)
        {
            return false;
        }
    }
    return true;
}

bool fulfills_delaunay(edge *e)
{
    if (e -> leftfaceLabel() == 0 or e -> rightfaceLabel() == 0)
        return true;
    point a = e -> originPosition();
    point b = e -> destinationPosition();
    point c = e -> fnext() -> destinationPosition();
    point d = e -> twin() -> fnext() -> destinationPosition();
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

/* Tests */

void test_random_point_location_in_random_triangulation(point_location &locator, int numPoints, bool delaunay)
{
    triangulation tr;
    int numCorrect = 0;

    int left = -10000000, top = 10000000, right = 10000000, bottom = -10000000;
    std::tuple <T, T, T, T> bounding_box{left, top, right, bottom};
    double padding_coeff = 1.3; // So that some points are outside bounding box;

    startTimer();
    if (!delaunay)
    {
        tr.generateRandomTriangulation(numPoints, arbitraryTriangulation, bounding_box);
        endTimer();
        print_time("Generating arbitrary random triangulation");
    }
    else
    {
        tr.generateRandomTriangulation(numPoints, delaunayTriangulation, bounding_box);
        endTimer();
        print_time("Generating delaunay random triangulation");
    }
    startTimer();
    locator.init(tr);
    endTimer();
    print_time("Time to construct locator");

    /* Check that each face has a unique label */
    std::vector <int> faces;
    for (edge* e: tr.traverse(dualGraph, traverseNodes))
    {
        faces.push_back(e -> origin().getLabel());
    }
    std::sort(faces.begin(), faces.end());
    bool failure = false;
    for (int i = 0; i < faces.size(); i++)
    {
        if (i != faces[i])
            failure = true;
    }
    assert(!faces.empty());
    assert(!failure);

    uniform_point_rng rng(padding_coeff * left, padding_coeff * top, padding_coeff * right, padding_coeff * bottom);
    std::vector <point> locating = rng.getRandom(numPoints);

    startTimer();
    for (point p: locating)
    {
        edge* e = locator.locate(p);
        bool expected_in_box = in_padded_bounding_box(p, left, top, right, bottom);
        bool found_in_box = (e != nullptr);
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
    triangulation tr;
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

void write_delaunay_triangulation(int numPoints, const std::string &file_name)
{
    std::ofstream writer(file_name);
    triangulation tr;
    tr.write_random_delaunay_triangulation(numPoints, writer);
    writer.close();
}

void test_saving_delaunay_triangulation(int numPoints)
{
    int numCorrect = 0, total = 0;

    write_delaunay_triangulation(numPoints, "temp.txt");

    std::ifstream reader("temp.txt");
    assert(reader.is_open());
    triangulation tr;
    tr.read_OFF_file(reader);
    reader.close();
    for (edge* e: tr.traverse(primalGraph, traverseEdges))
    {
        total++;
        if (fulfills_delaunay(e))
            numCorrect++;
    }
    int np = tr.traverse(primalGraph, traverseNodes).size();
    assert(np == 4 + numPoints);
    print_percent_correct("test_saving_delaunay_triangulation", numCorrect, total);
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

    bool allSame = true;
    uniform_point_rng rng2(left, top, right, bottom);
    for (int i = 0; i < numPoints; i++)
    {
        point p = rng.getRandom();
        point p2 = rng2.getRandom();
        if (p != p2)
            allSame = false;
    }

    double threshold = 1.01;
    double x_rat = (double) maxx / minx;
    double y_rat = (double) maxy / miny;
    int passed, total;
    passed = total = 3;
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
    if (allSame)
    {
        std::cout << "Different generators produce same sequence of numbers" << std::endl;
        --passed;
    }
    print_percent_correct("test_rng_distribution", passed, total);
    endTimer();
}

int main()
{
    /* Rng Checking */

    test_rng_distribution();
    print_time("test_rng_distribution");

    /* Delaunay Storage Testing */
    test_saving_delaunay_triangulation(1000);

    /* Point Location Testing */

    int numPoints = 100000;

    /* Naive Quadtree */
    naive_quadtree quad_locator(90, 60);

    test_random_point_location_in_random_triangulation(quad_locator, numPoints, true);
    print_time("test_random_point_location_in_random_delaunay_triangulation quad");

    /*
    test_random_point_location_in_random_triangulation(quad_locator, numPoints, false);
    print_time("test_random_point_location_in_random_arbitrary_triangulation quad");
    */

    /* Slab decomposition */

    slab_decomposition slab_locator;

    test_random_point_location_in_random_triangulation(slab_locator, numPoints, true);
    print_time("test_random_point_location_in_random_delaunay_triangulation slab");

    /*
    test_random_point_location_in_random_triangulation(slab_locator, numPoints, false);
    print_time("test_random_point_location_in_random_arbitrary_triangulation slab");
    */

    /* Oriented Walk */

    std::unique_ptr <walking_scheme> locator_ptr = std::make_unique<lawson_oriented_walk>(lawson_oriented_walk({stochasticWalk, fastRememberingWalk}, std::pow(numPoints, 1.0/4.0)));
    std::unique_ptr <starting_edge_selector> selector_ptr = std::make_unique<starting_edge_selector>(starting_edge_selector(selectSample, std::pow(numPoints, 1.0/3.0)));
    walking_point_location walk_locator(locator_ptr, selector_ptr);

    test_random_point_location_in_random_triangulation(walk_locator, numPoints, true);
    print_time("test_random_point_location_in_random_delaunay_triangulation walking");

    /*
    test_random_point_location_in_random_triangulation(walk_locator, numPoints, false);
    print_time("test_random_point_location_in_random_arbitrary_triangulation walking");
    */

    /* Delaunay Speed Testing */

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
