#ifndef INTCALCULATOR_CONF_H
#define INTCALCULATOR_CONF_H

#include <fstream>
#include <sstream>
#include <tuple>
#include <vector>
#include <string>

struct config
{
    double abs_error;
    double rel_error;
    int n_threads;
    std::tuple<double, double> x_arr;
    std::tuple<double, double> y_arr;
    size_t size;
    std::vector<double> coeff;
    std::vector<double> base_val1;
    std::vector<double> base_val2;

    int readFromFile(std::ifstream &file);
    int check(int inst_check);
};

#endif //INTCALCULATOR_CONF_H
