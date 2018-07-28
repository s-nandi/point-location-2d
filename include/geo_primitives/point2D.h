#ifndef POINT2D_H_DEFINED
#define POINT2D_H_DEFINED

typedef double T;

class point2D
{
public:
    T x, y;

    point2D(){};
    point2D(T a, T b) {x = a, y = b;}

    bool operator < (const point2D&) const;
    bool operator == (const point2D&) const;
    bool operator != (const point2D&) const;

    point2D operator + (const point2D&) const;
    point2D operator - (const point2D&) const;
    friend point2D operator * (T, const point2D&);

    friend T cross(const point2D&, const point2D&);
    friend T dot(const point2D&, const point2D&);
    friend T orientation(const point2D&, const point2D&, const point2D&);

    friend std::istream& operator >> (std::istream&, point2D&);
    friend std::ostream& operator << (std::ostream&, const point2D&);
};

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

T cross(const point2D &a, const point2D &b)
{
    return a.x * b.y - a.y * b.x;
}

T dot(const point2D &a, const point2D &b)
{
    return a.x * b.x + a.y * b.y;
}

// Result is > 0 if (a, b, c) clockwise, < 0 if counter-clockwise, 0 if collinear
// Absolute value of the returned value is twice the area spanned by the triangle abc
T orientation(const point2D &a, const point2D &b, const point2D &c)
{
    return cross(c - a, b - a);
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

#endif
