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
                                                           result(1) {}
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

void int_calculator::integrate(int step, int count)
{
    double d_x = (std::get<1>(int_config.x_arr) - std::get<0>(int_config.x_arr)) / step;
    double d_y = (std::get<1>(int_config.y_arr) - std::get<0>(int_config.y_arr)) / step;
    double x, y, res = 0;
    int jump = 1;

    // std::cout << std::get<0>(int_config.x_arr) << " " << std::get<1>(int_config.x_arr) << std::endl;

    for (x = std::get<0>(int_config.x_arr); x < std::get<1>(int_config.x_arr); x += d_x)
    {
        jump = (count == 2) ? (jump + 1) % 2 : 0;
        for (y = std::get<0>(int_config.y_arr) + (count - 1 - jump) * d_y; y <= std::get<1>(int_config.y_arr); y += d_y * (count - jump))
            res += function(x, y);
    }
    result = res * d_x * d_y;
}

void int_calculator::find_best_integral(config &configs)
{
    double x_prev = std::get<0>(configs.x_arr);
    double x_step = (std::get<1>(configs.x_arr) - std::get<0>(configs.x_arr)) / configs.n_threads;
    std::vector<int_calculator> calcs;

    for (size_t i = 0; i < configs.n_threads; i++, x_prev += x_step)
    {
        config new_configs = configs;
        new_configs.x_arr = std::make_tuple(x_prev, x_prev + x_step);
        calcs.push_back(int_calculator(new_configs));
    }

    auto start = get_current_time_fenced();
    double step = 500;
    double prev_res = 0;
    double res = int_calculator::integrate_threads(calcs, step, 1);
    double abs_err = fabs(res - prev_res);
    double rel_err = fabs((res - prev_res) / res);

    while (int_calculator::abs_error(abs_err, configs) && int_calculator::rel_error(rel_err, configs))
    {
        prev_res = res;
        res = prev_res / 4;
        step *= 2;
        res += int_calculator::integrate_threads(calcs, step, 1);

        abs_err = fabs(res - prev_res);
        rel_err = fabs((res - prev_res) / res);
    }
    auto time_taken = get_current_time_fenced() - start;

    std::cout << "Result: " << res << std::endl;
    std::cout << "Abs err : rel err " << abs_err << " : " << rel_err << std::endl;
    std::cout << "Time: " << to_us(time_taken) << "mcs\n";
}

double int_calculator::integrate_threads(std::vector<int_calculator> &calcs, double step, double count)
{
    std::vector<std::thread> threads;
    for (int i = 0; i < calcs.size(); ++i)
    {
        calcs[i]._helper(threads, step, count);
        // auto func = calcs[i].integrate;
        // threads.emplace_back(std::ref(func), step, count);
    }
    for (auto &t : threads)
        t.join();
    double res = 0;
    for (auto &calc : calcs)
        res += calc.result;
    return res;
}

void int_calculator::_helper(std::vector<std::thread> &threads, double step, double count)
{
    threads.emplace_back(this->integrate, step, count);
}

bool int_calculator::rel_error(double rel_err, config &conf)
{
    return rel_err > conf.rel_error;
}

bool int_calculator::abs_error(double abs_err, config &conf)
{
    return abs_err > conf.abs_error;
}
