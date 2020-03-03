#include "integral.h"

integralCalculator::integralCalculator() {
    integral_conf = new config();
    prev_result = 0;
    result = 0;
}
integralCalculator::integralCalculator(config& other_config):
    integral_conf(other_config),
    prev_result(0),
    result(0) { }
integralCalculator::integralCalculator(integralCalculator& other):
    integral_conf(other . integral_conf),
    result(other.result) { }
integralCalculator::~integralCalculator() { }

double integralCalculator::function(double x1, double x2, int m, std::vector<double> &a1, std::vector<double> &a2, std::vector<double> &c)
{
    double sum = 0;
    for (auto i = 0; i < m; ++i)
    {
        sum += c[i] * exp(-(1 / M_PI) * (pow(x1 - a1[i], 2) + pow(x2 - a2[i], 2))) * cos(M_PI * (pow(x1 - a1[i], 2) + pow(x2 - a2[i], 2)));
    }
    return -sum;
}

