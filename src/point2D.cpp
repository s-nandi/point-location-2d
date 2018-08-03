#include "geo_primitives/point2D.h"

/* Point Comparison */

bool point2D::operator < (const point2D &o) const
{
    if (x != o.x) return x < o.x;
    else return y < o.y;
}

bool point2D::operator == (const point2D &o) const
{
    return x == o.x and y == o.y;
}

bool point2D::operator != (const point2D &o) const
{
    return !(*this == o);
}

/* Vector operations */

point2D point2D::operator + (const point2D &o) const
{
    return point2D(x + o.x, y + o.y);
}

point2D point2D::operator - (const point2D &o) const
{
    return point2D(x - o.x, y - o.y);
}

point2D operator * (T k, const point2D &p)
{
    return point2D(k * p.x, k * p.y);
}

point2D operator / (const point2D &p, T k)
{
    return point2D(p.x / k, p.y / k);
}

T cross(const point2D &a, const point2D &b)
{
    return a.x * b.y - a.y * b.x;
}

T dot(const point2D &a, const point2D &b)
{
    return a.x * b.x + a.y * b.y;
}

T point2D::magnitudeSquared()
{
    return dot(*this, *this);
}

// Result is > 0 if (a, b, c) clockwise, < 0 if counter-clockwise, 0 if collinear
// Absolute value of the returned value is twice the area spanned by the triangle abc
T orientation(const point2D &a, const point2D &b, const point2D &c)
{
    return cross(c - a, b - a);
}

// Result is > 0 if p is inside triangle abc, < 0 if outside, 0 if on the boundary
T inCircle(const point2D &p, const point2D &a, const point2D &b, const point2D &c)
{
    return dot(a, a) * cross(c - b, p - b) -
           dot(b, b) * cross(c - a, p - a) +
           dot(c, c) * cross(b - a, p - a) -
           dot(p, p) * cross(b - a, c - a);
}

/* Point IO */

std::istream& operator >> (std::istream &is, point2D &p)
{
    return is >> p.x >> p.y;
}

std::ostream& operator << (std::ostream &os, const point2D &p)
{
    return os << '(' << p.x << ", " << p.y << ')';
}