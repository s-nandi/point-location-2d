#ifndef POINT2D_H_DEFINED
#define POINT2D_H_DEFINED

#include <iostream>

typedef long double T;

class point2D
{
public:
    T x, y;

    point2D(){};
    point2D(T a, T b) {x = a, y = b;}

    bool operator < (const point2D&) const;
    bool operator > (const point2D&) const;
    bool operator == (const point2D&) const;
    bool operator != (const point2D&) const;

    point2D operator + (const point2D&) const;
    point2D operator - (const point2D&) const;
    friend point2D operator * (T, const point2D&);
    friend point2D operator / (const point2D&, T);
    T magnitudeSquared();

    friend T cross(const point2D&, const point2D&);
    friend T dot(const point2D&, const point2D&);
    friend T orientation(const point2D&, const point2D&, const point2D&);
    friend T inCircle(const point2D&, const point2D&, const point2D&, const point2D&);

    friend bool inSegment(point2D line[2], point2D p);
    friend bool intersects(point2D line1[2], point2D line2[2]);

    friend std::istream& operator >> (std::istream&, point2D&);
    friend std::ostream& operator << (std::ostream&, const point2D&);
};

#endif
