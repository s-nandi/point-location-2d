#include "point_location/non_walking/slab_decomposition.h"
#include "planar_structure/plane.h"
#include <set>
#include <algorithm>
#include <assert.h>
#include <iostream>

/* Event Functions */

// Returns position of event, assuming that edge is directed from left to right
point slab_decomposition::event::position() const
{
    if (isLeft)
        return segment -> originPosition();
    else
        return segment -> destinationPosition();
}

// Comparator for two edge pointers that compares the y coordinate of their left endpoints
// Used to sort segments within a slab
bool slab_decomposition::event::operator < (const event &o) const
{
    return position() < o.position();
}

/* Slab Decomposition Implementation */

void slab_decomposition::init(plane &p)
{
    slabs.clear();
    slab_positions.clear();

    // Each event is a pair corresponding to the position of a vertex and the edge it belongs to, from left to right
    // Events will be used to line sweep from left to right and create slabs for each distinct pair of consecutive x-coordinates
    std::vector <event> events;
    std::vector <int> x_coordinates;

    std::vector <edge*> edges = p.traverse(primalGraph, traverseEdges);
    for (int i = 0; i < edges.size(); i++)
    {
        point origin = edges[i] -> originPosition();
        point destination = edges[i] -> destinationPosition();
        // If origin is to the right of destination, flip the edge
        if (origin > destination)
        {
            edges[i] = edges[i] -> twin();
            std::swap(origin, destination);
        }
        events.push_back({edges[i], true});
        events.push_back({edges[i], false});
        x_coordinates.push_back(origin.x);
        x_coordinates.push_back(destination.x);
    }
    std::sort(events.begin(), events.end());
    std::sort(x_coordinates.begin(), x_coordinates.end());
    x_coordinates.erase(std::unique(x_coordinates.begin(), x_coordinates.end()), x_coordinates.end());

    // Comparator for two edge pointers that compares the y coordinate of their left endpoints
    // Used to sort segments within a slab
    auto compareByY = [](const event &a, const event &b)
    {
        point a_pos = a.position();
        point b_pos = b.position();

        if (a_pos.y != b_pos.y) return a_pos.y < b_pos.y;
        else if (a_pos.x != b_pos.x) return a_pos.x < b_pos.x;
        else return a.segment < b.segment;
    };

    int event_it = 0;
    std::set <event, decltype(compareByY)> current_slab(compareByY);
    for (int i = 0; i < x_coordinates.size(); i++)
    {
        while (event_it < events.size() and events[event_it].position().x == x_coordinates[i])
        {
            // Left endpoints represent insertion events
            if (events[event_it].isLeft)
                current_slab.insert(events[event_it]);
            // Right endpoints represent removal events
            else
                current_slab.erase(events[event_it]);
            ++event_it;
        }
        std::vector <edge*> slab_edges;
        for (event e: current_slab)
        {
            slab_edges.push_back(e.segment);
        }
        slabs.push_back(slab_edges);
        slab_positions.push_back(x_coordinates[i]);
    }
}

// Finds the index of the slab that p belongs to
// If p is not contained in any slab, returns -1
int slab_decomposition::findSlabIndex(point p)
{
    if (p.x < slab_positions[0] or p.x > slab_positions.back()) return -1;

    int l = 0, r = slab_positions.size() - 1;
    while (l < r)
    {
        int m = (l + r) / 2;
        if (p.x < slab_positions[m])
            r = m - 1;
        else if (!(m + 1 < slabs.size()) or p.x <= slab_positions[m + 1])
            return m;
        else
            l = m + 1;
    }
    return l;
}

edge* slab_decomposition::findInSlab(int index, point p)
{
    // Returns the y-coordinate of an edges left endpoint
    // Assumes that e is directed from left to right
    // Used to detect if point is above/below edge since we know edges don't intersect within a slab
    auto getY = [](edge* e)
    {
        return e -> originPosition().y;
    };

    assert(p.x >= slab_positions[index]);
    assert(index + 1 >= slab_positions.size() or p.x <= slab_positions[index + 1]);

    if (p.y < getY(slabs[index][0]) or p.y > getY(slabs[index].back()))
        return NULL;

    int l = 0, r = slabs[index].size() - 1;
    while (l < r)
    {
        int m = (l + r) / 2;

        if (p.y < getY(slabs[index][m]))
            r = m - 1;
        else if (!( m + 1 < slabs[index].size()) or p.y <= getY(slabs[index][m + 1]))
            return slabs[index][m];
        else
            l = m + 1;
    }
    return slabs[index][l];
}

edge* slab_decomposition::locate(point p)
{
    int ind = findSlabIndex(p);
    if (ind == -1) return NULL;

    edge* bounding_edge = findInSlab(ind, p);

    if (bounding_edge == NULL)
        return NULL;

    point origin = bounding_edge -> originPosition();
    point destination = bounding_edge -> destinationPosition();

    if (orientation(origin, destination, p) > 0)
        return bounding_edge -> twin();
    else
        return bounding_edge;
}
