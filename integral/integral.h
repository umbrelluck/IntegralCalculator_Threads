#ifndef INTCALCULATOR_INTEGRAL_H
#define INTCALCULATOR_INTEGRAL_H

#include "../config/config.h"

#include <vector>
#include <cmath>

struct integralCalculator{
    config integral_conf;
    double prev_result;
    double result;

    integralCalculator();
    integralCalculator(config& other_config);
    integralCalculator(integralCalculator& other);
    ~integralCalculator();

    double function(double x1, double x2, int m, std::vector<double> &a1, std::vector<double> &a2, std::vector<double> &c);




};

#endif //INTCALCULATOR_INTEGRAL_H
