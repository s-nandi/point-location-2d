#include <iostream>
#include <iomanip>

#include "plane.h"
#include "debug.h"

int main()
{
    std::cout << std::fixed << std::setprecision(9);
    plane p;
    try
    {
        p.read_OFF_file(std::cin);
        p.interactiveTour(std::cin, std::cout);
    }
    catch (std::exception &e)
    {
        std::cout<< e.what() <<std::endl;
    }

    return 0;
}
