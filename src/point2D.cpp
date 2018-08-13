#include "geo_primitives/point2D.h"

/* Point Comparison */

bool point2D::operator < (const point2D &o) const
{
    if (x != o.x) return x < o.x;
    else return y < o.y;
}

bool point2D::operator > (const point2D &o) const
{
    return o < *this;
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

/* Geometry Predicates */

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

// Returns true if p is on line m
// Being on m's endpoints counts as being on m
bool inSegment(point2D m[2], point2D p)
{
    point2D ab = m[1] - m[0], a = m[0];

    int cross_ab_ap = cross(ab, p - a);
    if (cross_ab_ap == 0) //check if a, b, p are collinear
    {
        int dot_ab_ap = dot(ab, p - a);
        bool oppositeDirection = dot_ab_ap < 0; //true if p is strictly on other side of a than b is
        bool tooFar = dot_ab_ap > ab.magnitudeSquared(); //true if p is strictly further from a than b is
        return !oppositeDirection and !tooFar;
    }
    else return false;
}

// Checks if two line segments intersect by checking if either segment lies entirely on one side of the other line
// If both lines are collinear, checks if either endpoint of a segment lies on the other segment
bool intersects(point2D m[2], point2D n[2])
{
    T cross_m_n = cross(m[1] - m[0], n[1] - n[0]);
    if (cross_m_n == 0)
        return inSegment(m, n[0]) or inSegment(m, n[1]);
    else
    {
        bool diffside_m = orientation(m[0], m[1], n[0]) * orientation(m[0], m[1], n[1]) < 0; //diff side iff one ccw and one cw orientation
        bool diffside_n = orientation(n[0], n[1], m[0]) * orientation(n[0], n[1], m[1]) < 0;
        return diffside_m and diffside_n;
    }
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
