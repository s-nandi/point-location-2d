#include "point_location/non_walking/naive_quadtree.h"
#include "planar_structure/plane.h"

naive_quadtree::naive_quadtree(int overlapBound, int depthBound)
{
    MAX_OVERLAP = overlapBound;
    MAX_DEPTH = depthBound;
}

void naive_quadtree::init(plane& pln)
{
    T box_left, box_top, box_right, box_bottom;
    std::tie(box_left, box_top, box_right, box_bottom) = pln.bounds;

    long long left = -1, top = 1, right = 1, bottom = -1;
    while (left > box_left or top < box_top or right < box_right or bottom > box_bottom)
        left *= 2, top *= 2, right *= 2, bottom *= 2;

    root = quadtree(std::make_tuple(left, top, right, bottom));
    root.setParameters(MAX_OVERLAP, MAX_DEPTH);

    for (edge* face: pln.traverse(dualGraph, traverseNodes))
    {
        if (face -> origin().getLabel() == 0) continue;
        root.insert(face);
    }

    auto dimension = getDimensions();
    std::cout << "Quadtree Dimensions -> Num Nodes: " << dimension.first << " Depth: " << dimension.second << std::endl;
}

edge* naive_quadtree::locate(point p)
{
    return root.locate(p);
}

// Returns number of nodes in quadtree along with the depth of the lowest node of the quadtree
std::pair <int, int> naive_quadtree::getDimensions()
{
    return {root.getNumNodes(), root.getDepth()};
}
