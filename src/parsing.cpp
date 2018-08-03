#include "parsing.h"
#include "quadedge_structure/vertex.h"
#include <tuple>
#include <sstream>

std::pair <std::vector <point>, std::vector <std::vector<int>>> parse_OFF_file(std::istream &is)
{
    int numPoints, numFaces, numEdges;
    std::vector <point> points;
    std::vector <std::vector<int>> faces;

    // Stages: 0 (read header), 1 (read vertex/face/edge count), 2 (read vertices), 3 (read faces)
    int stage = 0;
    // Keeps track of how many lines were read at the current stage
    int linesRead = 0;
    while (stage <= 3)
    {
        std::string line;
        std::getline(is, line);
        std::istringstream iss(line);

        // Ignore comments or blank lines
        if (line.length() == 0 or line[0] == '#')
            continue;

        switch (stage)
        {
            case 0:
            {
                std::string header;
                iss >> header;
                // File must start with OFF
                if (header != "OFF")
                {
                    throw incorrectHeaderException();
                }
                stage = 1;
                break;
            }
            case 1:
            {
                iss >> numPoints >> numFaces >> numEdges;
                points.resize(numPoints);
                faces.resize(numFaces);
                stage = 2;
                break;
            }
            case 2:
            {
                iss >> points[linesRead];
                linesRead++;
                // If numPoints points were read, move to next stage
                if (linesRead == numPoints)
                {
                    stage = 3;
                    linesRead = 0;
                }
                break;
            }
            case 3:
            {
                int n;
                iss >> n;
                faces[linesRead].resize(n);
                for (int i = 0; i < n; i++)
                {
                    iss >> faces[linesRead][i];
                }
                linesRead++;
                // If numFaces faces were read, move to next stage
                if (linesRead == numFaces)
                {
                    stage = 4;
                    linesRead = 0;
                }
                break;
            }
        }
    }
    return {points, faces};
}

std::vector <point> parse_PT_file(std::istream &is)
{
    std::vector <point> points;
    while (true)
    {
        std::string line;
        getline(is, line);
        std::istringstream iss(line);
        point p;
        iss >> p;
        points.push_back(p);
    }
    return points;
}
