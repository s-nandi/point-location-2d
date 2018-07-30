#ifndef POINT3D_H_DEFINED
#define POINT3D_H_DEFINED

#include <iostream>

typedef double T;

class point3D
{
public:
    T x, y, z;

    point3D(){};
    point3D(T a, T b, T c) {x = a, y = b, z = c;}

    bool operator < (const point3D&) const;
    bool operator == (const point3D&) const;
    bool operator != (const point3D&) const;

    point3D operator + (const point3D&) const;
    point3D operator - (const point3D&) const;
    friend point3D operator * (T, const point3D&);

    friend point3D cross(const point3D&, const point3D&);
    friend T dot(const point3D&, const point3D&);
    friend T orientation(const point3D&, const point3D&, const point3D&);

    friend std::istream& operator >> (std::istream&, point3D&);
    friend std::ostream& operator << (std::ostream&, const point3D&);
};

#endif
