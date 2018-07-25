#ifndef POINT_H_DEFINED
#define POINT_H_DEFINED

typedef double T;

class point
{
public:
    T x, y;

    point(){};
    point(T a, T b) {x = a, y = b;}

    bool operator < (const point &o) const;
    bool operator == (const point &o) const;
    bool operator != (const point&) const;

    point operator + (const point&) const;
    point operator - (const point&) const;

    friend T cross(const point&, const point&);
    friend T dot(const point&, const point&);
    friend T doubleOrientedArea(const point&, const point&, const point&);

    friend std::istream& operator >> (std::istream&, point&);
    friend std::ostream& operator << (std::ostream&, const point&);
};

/* Point Comparison */

bool point::operator < (const point &o) const
{
    if (x != o.x) return x < o.x;
    else return y < o.y;
}

bool point::operator == (const point &o) const
{
    return x == o.x and y == o.y;
}

bool point::operator != (const point &o) const
{
    return !(*this == o);
}

/* Vector operations */

point point::operator + (const point &o) const
{
    return point(x + o.x, y + o.y);
}

point point::operator - (const point &o) const
{
    return point(x - o.x, y - o.y);
}

T cross(const point &a, const point &b)
{
    return a.x * b.y - a.y * b.x;
}

T dot(const point &a, const point &b)
{
    return a.x * b.x + a.y * b.y;
}

// Returns > 0 if (a, b, c) clockwise, < 0 if counter-clockwise, 0 if collinear
T doubleOrientedArea(const point &a, const point &b, const point &c)
{
    return cross(c - a, b - a);
}

/* Point IO */

std::istream& operator >> (std::istream &is, point &p)
{
    return is >> p.x >> p.y;
}

std::ostream& operator << (std::ostream &os, const point &p)
{
    return os << '(' << p.x << ", " << p.y << ')';
}

#endif
