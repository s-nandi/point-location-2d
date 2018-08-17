#ifndef TESTING_H_DEFINED
#define TESTING_H_DEFINED

#include <chrono>
#include <iostream>

/* Helper Functions for Timing */

using clock_type = std::chrono::high_resolution_clock;
using duration_type =  std::chrono::duration<double>;
using time_type = std::chrono::time_point<clock_type>;

clock_type timer;
time_type start_time, end_time;

inline void startTimer()
{
    start_time = clock_type::now();
}

inline double endTimer()
{
    end_time = clock_type::now();
    duration_type duration = end_time - start_time;
    return duration.count();
}

/* Helper Functions for Printing Timer and Correctness*/

void print_time(const std::string &name, double t)
{
    std::cout << "Time taken for " << name << ": " << t << " s" << std::endl;
}

void print_time(const std::string &name)
{
    std::cout << "Time taken for " << name << ": " << ((duration_type) (end_time - start_time)).count() << " s" << std::endl;
}

void print_percent_correct(const std::string &name, int correct, int total)
{
    double percentage = (double) (correct * 100) / total;
    std::cout << name << ": " << correct << " / " << total << " (" << percentage << "%)" << std::endl;
}

#endif
