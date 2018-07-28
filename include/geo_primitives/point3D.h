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

/* Point Comparison */

bool point3D::operator < (const point3D &o) const
{
    if (x != o.x) return x < o.x;
    else if (y != o.y) return y < o.y;
    else return z < o.z;
}

bool point3D::operator == (const point3D &o) const
{
    return x == o.x and y == o.y and z == o.z;
}

bool point3D::operator != (const point3D &o) const
{
    return !(*this == o);
}

/* Vector operations */

point3D point3D::operator + (const point3D &o) const
{
    return point3D(x + o.x, y + o.y, z + o.z);
}

point3D point3D::operator - (const point3D &o) const
{
    return point3D(x - o.x, y - o.y, z - o.z);
}

point3D operator * (T k, const point3D &p)
{
    return point3D(k * p.x, k * p.y, k * p.z);
}

point3D cross(const point3D &a, const point3D &b)
{
    return point3D(a.y * b.z - b.y * a.z,
                   b.x * a.z - a.x * b.z,
                   a.x * b.y - b.x * a.y);
}

T dot(const point3D &a, const point3D &b)
{
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

// Result is > 0 if (a, b, c) clockwise, < 0 if counter-clockwise, 0 if collinear when viewed from above
// Assumes plane containing triangle abc is not vertical -> ie. normal of plane has a non-zero z-component
T orientation(const point3D &a, const point3D &b, const point3D &c)
{
    point3D normal = cross(c - a, b - a);
    return normal.z;
}

/* Point IO */

std::istream& operator >> (std::istream &is, point3D &p)
{
    return is >> p.x >> p.y >> p.z;
}

std::ostream& operator << (std::ostream &os, const point3D &p)
{
    return os << '(' << p.x << ", " << p.y << ", " << p.z << ')';
}

#endif
