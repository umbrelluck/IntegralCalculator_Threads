#include <atomic>
#include <chrono>
#include <iostream>
#include <fstream>
#include <string>
#include <cassert>

#include "integral/integral.h"

inline std::chrono::steady_clock::time_point get_current_time_fenced()
{
    assert(std::chrono::steady_clock::is_steady &&
           "Timer should be steady (monotonic).");
    std::atomic_thread_fence(std::memory_order_seq_cst);
    auto res_time = std::chrono::steady_clock::now();
    std::atomic_thread_fence(std::memory_order_seq_cst);
    return res_time;
}

template <class D>
inline long long to_us(const D &d)
{
    return std::chrono::duration_cast<std::chrono::microseconds>(d).count();
}

double function(double x1, double x2, int m, std::vector<double> &a1, std::vector<double> &a2, std::vector<double> &c)
{
    double sum = 0;
    for (auto i = 0; i < m; ++i)
    {
        sum += c[i] * exp(-(1 / M_PI) * (pow(x1 - a1[i], 2) + pow(x2 - a2[i], 2))) * cos(M_PI * (pow(x1 - a1[i], 2) + pow(x2 - a2[i], 2)));
    }
    return -sum;
}

bool rel_error(double previous, double res, config &con)
{
    return fabs((previous - res) / res) > con.rel_error;
}

bool abs_error(double previous, double res, config &con)
{
    return fabs(previous - res) > con.abs_error;
}

template <typename fun_T>
double integrate(fun_T fun, config &configs, int step, int count)
{
    double d_x = (std::get<1>(configs.x_arr) - std::get<0>(configs.x_arr)) / step;
    double d_y = (std::get<1>(configs.y_arr) - std::get<0>(configs.y_arr)) / step;
    double x, y, res = 0;
    // double tmp;
    int jump = 1;

    for (x = std::get<0>(configs.x_arr); x <= std::get<1>(configs.x_arr); x += d_x)
    {
        jump = (count == 2) ? (jump + 1) % 2 : 0;
        for (y = std::get<0>(configs.y_arr) + (count - 1 - jump) * d_y; y <= std::get<1>(configs.y_arr); y += d_y * (count - jump))
        {
            // if (x == -5)
            // std::cout << "\n";
            res += fun(x, y, configs.size, configs.base_val1, configs.base_val2, configs.coeff) * d_x * d_y;
        }
    }
    return res;
}

template <typename fun_T>
double find_best_integral(fun_T fun, config &configs)
{
    double previous = 0;
    int count = 1;
    size_t step = 2;
    bool to_continue = true;
    double rel_err, abs_err;
    int error_index = (configs.rel_error != 0) ? 0 : 1;

    typedef bool (*fn)(double, double, config &);
    fn errors[] = {rel_error, abs_error};

    auto start = get_current_time_fenced();
    double res = integrate(fun, configs, step, count);
    count = 2;
    while (errors[error_index](previous, res, configs))
    // while (to_continue)
    {
        previous = res;
        res = previous / 4;
        step *= 2;
        res += integrate(fun, configs, step, count);

        // std::cout << previous << " " << res << " REl error is " << rel_err << "\n";
        // // to_continue = (abs_err > configs.abs_error);
        // to_continue = to_continue && (rel_err > configs.rel_error);
    }
    auto time_taken = get_current_time_fenced() - start;
    abs_err = fabs(res - previous);
    rel_err = fabs((res - previous) / res);
    std::cout << "\n---------------------------------------------\n";
    std::cout << "Result: " << res << std::endl;
    std::cout << "Abs err : rel err " << abs_err << " : " << rel_err << std::endl;
    std::cout << "Time: " << to_us(time_taken) << "mcs\n";
    return res;
}

int main(int argc, char **argv)
{
    std::cout << "\n";
    std::string fileName;
    if (argc == 1)
    {
        std::cout << "  ! No file specified\n";
        std::cout << "    => Using default name 'example.config'\n";
        fileName = "example.config";
    }
    else if (argc > 2)
    {
        std::cout << "  ! Incorrect usage of the program (to many arguments)\n"
                  << "    Correct is ./intCalculator [FILE]\n";
        return 1;
    }
    else
        fileName = argv[1];
    std::ifstream file(fileName);
    if (!file)
    {
        std::cout << "  ! No such file\n";
        return 1;
    }
    config configs;
    int inst_check = configs.readFromFile(file);
    if (configs.check(inst_check))
    {
        std::cout << "  ! Error in configuration file\n";
        return 1;
    }
    auto* calculator = new integralCalculator(configs);

    double value = find_best_integral(function, configs);
    // std::cout << "Result is " << value << "\n";

    return 0;
}
/* 


1. function x2 faster - done 
2. errors in reading
3. time - done
4. rel and abs error
5. size = size_t - done



 */