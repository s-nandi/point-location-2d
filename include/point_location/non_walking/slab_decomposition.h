#ifndef SLAB_DECOMPOSITION_H_DEFINED
#define SLAB_DECOMPOSITION_H_DEFINED

#include <vector>
#include "point_location/point_location.h"

class slab_decomposition : public point_location
{
private:
    struct event;
    std::vector <std::vector <edge*>> slabs;
    std::vector <int> slab_positions;

    int findSlabIndex(point);
    edge* findInSlab(int, point);
public:
    void init(plane&);
    edge* locate(point);
};

struct slab_decomposition::event
{
    edge* segment;
    bool isLeft;

    point position() const;
    bool operator < (const event&) const;
};

#endif
