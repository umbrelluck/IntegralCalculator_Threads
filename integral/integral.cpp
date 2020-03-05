#include "integral.h"

#include <iostream>

inline std::chrono::steady_clock::time_point int_calculator::get_current_time_fenced()
{
    assert(std::chrono::steady_clock::is_steady &&
           "Timer should be steady (monotonic).");
    std::atomic_thread_fence(std::memory_order_seq_cst);
    auto res_time = std::chrono::steady_clock::now();
    std::atomic_thread_fence(std::memory_order_seq_cst);
    return res_time;
}
template <class D>
inline long long int_calculator::to_us(const D &d)
{
    return std::chrono::duration_cast<std::chrono::microseconds>(d).count();
}

int_calculator::int_calculator(const config confstruct) : prev_result(0),
                                                           result(0),
                                                           rel_err(1),
                                                           abs_err(1),
                                                           int_config(confstruct) {}
int_calculator::~int_calculator() = default;

double int_calculator::function(double x1, double x2)
{
    double sum = 0;
    for (size_t i = 0; i < int_config.size; ++i)
    {
        sum += int_config.coeff[i] * exp(-(1 / M_PI) * (pow(x1 - int_config.base_val1[i], 2) + pow(x2 - int_config.base_val2[i], 2))) * cos(M_PI * (pow(x1 - int_config.base_val1[i], 2) + pow(x2 - int_config.base_val2[i], 2)));
    }
    return -sum;
}

void int_calculator::find_best_integral(int_calculator &calc)
{
    int count = 1;
    size_t step = 2;

    auto start = get_current_time_fenced();
    calc.result = calc.integrate(step, count);
    count = 2;
    while (calc.abs_error(calc.abs_err, calc) && calc.rel_error(calc.rel_err, calc))
    {
        calc.prev_result = calc.result;
        calc.result = calc.prev_result / 4;
        step *= 2;
        calc.result += calc.integrate(step, count);

        calc.abs_err = fabs(calc.result - calc.prev_result);
        calc.rel_err = fabs((calc.result - calc.prev_result) / calc.result);
    }
    auto time_taken = get_current_time_fenced() - start;

    // std::cout << "Result: " << (*calc).result << std::endl;
    // std::cout << "Abs err : rel err " << (*calc).abs_err << " : " << (*calc).rel_err << std::endl;
    std::cout << "Time: " << to_us(time_taken) << "mcs\n";
}

double int_calculator::integrate(int step, int count)
{
    double d_x = (std::get<1>(int_config.x_arr) - std::get<0>(int_config.x_arr)) / step;
    double d_y = (std::get<1>(int_config.y_arr) - std::get<0>(int_config.y_arr)) / step;
    double x, y, res = 0;
    int jump = 1;

    for (x = std::get<0>(int_config.x_arr); x < std::get<1>(int_config.x_arr); x += d_x)
    {
        jump = (count == 2) ? (jump + 1) % 2 : 0;
        for (y = std::get<0>(int_config.y_arr) + (count - 1 - jump) * d_y; y <= std::get<1>(int_config.y_arr); y += d_y * (count - jump))
        {
            res += function(x, y);
        }
    }
    return res * d_x * d_y;
}

bool int_calculator::rel_error(double rel_err, int_calculator &calc)
{
    return rel_err > calc.int_config.rel_error;
}

bool int_calculator::abs_error(double abs_err, int_calculator &calc)
{
    return abs_err > calc.int_config.abs_error;
}
