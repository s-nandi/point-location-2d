#ifndef POINT_LOCATION_H_DEFINED
#define POINT_LOCATION_H_DEFINED

class plane;

class pointlocation
{
friend plane;
friend debug;
protected:
    int numTests, numFaces;

    plane* pl;
    edge* startingEdge;
    pointlocation(plane &pln) : numTests(0), numFaces(0), pl(&pln) {startingEdge = pln.incidentEdge;}
    virtual edge* locate(point) = 0;
};

#endif
