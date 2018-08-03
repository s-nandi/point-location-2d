#ifndef PARSING_H_DEFINED
#define PARSING_H_DEFINED

#include <vector>
#include <iostream>

// Thrown if OFF file does not begin with "OFF"
struct incorrectHeaderException : std::exception
{
    const char * what () const throw ()
    {
    	return "Incorrect File Header: Unable to Parse OFF File";
    }
};

class point2D;
typedef point2D point;

// Reads OFF file and returns point set along with a vector of faces, where each face is represented by the indices of their vertices
// Vertices are 0-indexed and are labeled in the order they appear in the point set
// Most functions assume that vertices in a face are listed in ccw order
std::pair <std::vector <point>, std::vector <std::vector<int>>> parse_OFF_file(std::istream&);

// Reads file containing a point on each line
// Number of points is not given in file
std::vector <point> parse_PT_file(std::istream&);

#endif
