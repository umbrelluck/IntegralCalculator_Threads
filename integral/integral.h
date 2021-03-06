#ifndef INTCALCULATOR_INTEGRAL_H
#define INTCALCULATOR_INTEGRAL_H

#include "../config/config.h"

#include <cmath>

#include <chrono>
#include <cassert>
#include <atomic>

class int_calculator {
public:
    double prev_result;
    double result;
    double rel_err;
    double abs_err;
    config int_config;

    int_calculator(const config& confstruct);
    ~int_calculator();

    double function(double x1, double x2);
    double integrate(int step, int count);

    static void find_best_integral(int_calculator& calc);

    static bool rel_error(int_calculator& inst);
    static bool abs_error(int_calculator& inst);

    static inline std::chrono::steady_clock::time_point get_current_time_fenced();

    template<class D>
    static inline long long to_us(const D &d);
};


#endif //INTCALCULATOR_INTEGRAL_H
