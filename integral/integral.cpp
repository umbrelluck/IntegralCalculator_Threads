#include "integral.h"
#include <cmath>
#include <iostream>
// #include <mutex>
// std::mutex m;

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

int_calculator::int_calculator(const config &confstruct) : int_config(confstruct),
                                                           result(1),
                                                           prev_result(0),
                                                           rel_err(1),
                                                           abs_err(1) {}
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

void int_calculator::integrate(int step)
{
    double d_x = ((std::get<1>(int_config.x_arr) - std::get<0>(int_config.x_arr)) * sqrt(int_config.n_threads)) / sqrt(step);
    double d_y = ((std::get<1>(int_config.y_arr) - std::get<0>(int_config.y_arr))) / sqrt(step);
    double x, y, res = 0;
    int jump = 1;

    // std::cout << std::get<0>(int_config.x_arr) << " " << std::get<1>(int_config.x_arr) << std::endl;

    for (x = std::get<0>(int_config.x_arr); x < std::get<1>(int_config.x_arr); x += d_x)
    {
        for (y = std::get<0>(int_config.y_arr); y <= std::get<1>(int_config.y_arr); y += d_y)
            res += function(x, y);
    }
    result = res * d_x * d_y;
}

void int_calculator::find_best_integral(config &configs)
{
    double x_prev = std::get<0>(configs.x_arr);
    double x_step = (std::get<1>(configs.x_arr) - std::get<0>(configs.x_arr)) / configs.n_threads;
    std::vector<int_calculator> calcs;

    for (size_t i = 0; i < configs.n_threads; i++)
    {
        config new_configs = configs;
        new_configs.x_arr = std::make_tuple(x_prev + x_step * i, x_prev + x_step * (i + 1));
        calcs.push_back(int_calculator(new_configs));
    }

    auto start = get_current_time_fenced();
    double step = 300;
    double prev_res = 0;
    double res = int_calculator::integrate_threads(calcs, step);
    double abs_err = fabs(res - prev_res);
    double rel_err = fabs((res - prev_res) / res);

    while ((int_calculator::abs_error(abs_err, configs) && int_calculator::rel_error(rel_err, configs)))
    {
        prev_res = res;
        // res = prev_res / 4;
        res = 0;
        step *= 2;
        res += int_calculator::integrate_threads(calcs, step);

        abs_err = fabs(res - prev_res);
        rel_err = fabs((res - prev_res) / res);

        // std::cout << "Steps " << step << "\n";
        // std::cout << "Abs err : rel err " << abs_err << " : " << rel_err << std::endl;
    }
    auto time_taken = to_us(get_current_time_fenced() - start);

    std::cout << "Result: " << res << "\n";
    std::cout << "Abs err : rel err " << abs_err << " : " << rel_err << "\n";
    std::cout << "Time: " << time_taken << "mcs\n";
}

double int_calculator::integrate_threads(std::vector<int_calculator> &calcs, double step)
{
    std::vector<std::thread> threads;
    size_t n = calcs.size();
    double step_tmp = step;
    for (auto i = 0; i < n; ++i)
    {
        // if (i < n - 1)
        threads.emplace_back(int_calculator::_helper, std::ref(calcs[i]), step);
        // else
        //     threads.emplace_back(int_calculator::_helper, std::ref(calcs[i]), step_tmp);
        // step_tmp -= step / n;
    }
    for (auto &t : threads)
        t.join();
    double res = 0;
    for (auto &calc : calcs)
        res += calc.result;
    return res;
}

void int_calculator::_helper(int_calculator &calc, double step)
{
    calc.integrate(step);
}

bool int_calculator::rel_error(double rel_err, config &conf)
{
    return rel_err > conf.rel_error;
}

bool int_calculator::abs_error(double abs_err, config &conf)
{
    return abs_err > conf.abs_error;
}
