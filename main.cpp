#include <iostream>
#include <iomanip>

#include "planar_structure/plane.h"
#include "planar_structure/triangulation.h"
#include "debug_functions.h"

int main()
{
    std::cout << std::fixed << std::setprecision(9);
    plane pln;
    try
    {
        pln.read_OFF_file(std::cin);
        debug::loud_print(std::cout, pln);
        debug::check_lawson(std::cin, std::cout, pln);
    }
    catch (std::exception &e)
    {
        std::cout<< e.what() <<std::endl;
    }

    return 0;
}
