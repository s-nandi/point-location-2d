#include "data_structures/quadtree.h"
#include "quadedge_structure/quadedge.h"
#include <assert.h>

quadtree::quadtree(const std::tuple <T, T, T, T>& bounding_box, int lev)
{
    std::tie(left, top, right, bottom) = bounding_box;
    // Make sure that dimensions are valid
    assert(left <= right and bottom <= top);
    for (int i = 0; i < 4; i++)
        children[i] = NULL;
    level = lev;
}

void quadtree::setParameters(int overlapBound, int depthBound)
{
    MAX_OVERLAP = overlapBound;
    MAX_DEPTH = depthBound;
}

bool quadtree::contains(const point &p)
{
    return p.x >= left and p.x <= right and p.y >= bottom and p.y <= top;
}

// face represents a dual edge outwards from the face we are checking for an overlap with this quadtree node
bool quadtree::overlaps(edge* face)
{
    bool triangleInsideSquare = true, squareInsideTriangle = true;
    std::vector <edge*> face_edges;
    for (auto it = face -> rot() -> begin(incidentOnFace); it != face -> rot() -> end(incidentOnFace); ++it)
    {
        face_edges.push_back(&*it);
        if (!contains(it -> originPosition()))
            triangleInsideSquare = false;
    }
    // If the face is strictly inside the square, they overlap
    if (triangleInsideSquare)
        return true;

    // Make sure that the face is a triangle
    assert(face_edges.size() == 3);
    // Make sure that the face is oriented ccw
    assert(orientation(face_edges[0] -> originPosition(), face_edges[1] -> originPosition(), face_edges[2] -> originPosition()) <= 0);

    point corners[4] = {{(T) left, (T) top},
                        {(T) left, (T) bottom},
                        {(T) right, (T) bottom},
                        {(T) right, (T) top}};
    for (int i = 0; i < 4; i++)
    {
        int inext = i + 1 < 4 ? i + 1 : 0;
        point square_edge[2] = {corners[i], corners[inext]};
        for (int j = 0; j < face_edges.size(); j++)
        {
            int jnext = j + 1 < face_edges.size() ? j + 1 : 0;
            point face_edge[2] = {face_edges[j] -> originPosition(), face_edges[jnext] -> originPosition()};
            if (intersects(face_edge, square_edge))
                return true;
            if (orientation(face_edge[0], face_edge[1], corners[i]) > 0)
                squareInsideTriangle = false;
        }
    }
    // If there are no intersections, they can only overlap if the square is strictly inside the triangle
    return squareInsideTriangle;
}

void quadtree::split()
{
    T midx = (left + right) / 2;
    T midy = (bottom + top) / 2;

    children[0] = new quadtree(std::make_tuple(left, top, midx, midy), level + 1);
    children[1] = new quadtree(std::make_tuple(left, midy, midx, bottom), level + 1);
    children[2] = new quadtree(std::make_tuple(midx, midy, right, bottom), level + 1);
    children[3] = new quadtree(std::make_tuple(midx, top, right, midy), level + 1);
    for (int i = 0; i < 4; i++)
        children[i] -> setParameters(MAX_OVERLAP, MAX_DEPTH);

    while (!faces.empty())
    {
        edge* face = faces.back();
        faces.pop_back();
        for (int i = 0; i < 4; i++)
        {
            if (children[i] -> overlaps(face))
                children[i] -> insert(face);
        }
    }
}

void quadtree::insert(edge* face)
{
    if (children[0] != NULL)
    {
        for (int i = 0; i < 4; i++)
        {
            if (children[i] -> overlaps(face))
                children[i] -> insert(face);
        }
    }
    else
    {
        faces.push_back(face);
        if (faces.size() == MAX_OVERLAP and level < MAX_DEPTH)
        {
            // Only split if the current cell is not a 1x1 cell
            if (right - left >= 2 and top - bottom >= 2)
                split();
        }
    }
}

edge* quadtree::locate(const point &p)
{
    if (children[0] != NULL)
    {
        for (int i = 0; i < 4; i++)
        {
            if (children[i] -> contains(p))
            {
                return children[i] -> locate(p);
            }
        }
        return NULL;
    }
    else
    {
        for (edge* face: faces)
        {
            bool allLeftTurns = true;
            for (auto it = face -> rot() -> begin(incidentOnFace); it != face -> rot() -> end(incidentOnFace); ++it)
            {
                // If edge forms a right turn with p, it cannot belong to the face containing p
                if (orientation(it -> originPosition(), it -> destinationPosition(), p) > 0)
                {
                    allLeftTurns = false;
                    break;
                }
            }
            if (allLeftTurns)
                return face -> rot();
        }
        return NULL;
    }
}

int quadtree::getNumNodes()
{
    if (children[0] == NULL)
        return faces.size();
    else
    {
        int sz = 0;
        for (int i = 0; i < 4; i++)
            sz += children[i] -> getNumNodes();
        return sz;
    }
}

int quadtree::getDepth()
{
    if (children[0] == NULL)
        return 0;
    else
    {
        int d = 0;
        for (int i = 0; i < 4; i++)
        {
            d = std::max(d, children[i] -> getDepth() + 1);
        }
        return d;
    }
}
