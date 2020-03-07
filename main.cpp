#include <iostream>
#include <fstream>
#include <thread>
#include <chrono>
#include <atomic>

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

int main(int argc, char **argv)
{
    std::cout << "-----------------\n";
    std::string fileName;
    if (argc == 1)
    {
        std::cout << " ! No file specified\n";
        std::cout << "  > Using default name 'example.config'\n";
        fileName = "example.conf";
    }
    else if (argc > 2)
    {
        std::cout << " ! Incorrect usage of the program (to many arguments)\n"
                  << "  > Correct is ./intCalculator [FILE]\n";
        return 1;
    }
    else
        fileName = argv[1];
    std::ifstream file(fileName);
    if (!file)
    {
        std::cout << " ! No such file\n";
        return 1;
    }
    config configs;
    int inst_check = configs.readFromFile(file);
    if (configs.check(inst_check))
    {
        std::cout << " ! Error in configuration file\n";
        return 1;
    }

    double x_prev = std::get<0>(configs.x_arr);
    double y_prev = std::get<0>(configs.y_arr);
    double x_step = (std::get<1>(configs.x_arr) - std::get<0>(configs.x_arr)) / configs.n_threads;
    double y_step = (std::get<1>(configs.y_arr) - std::get<0>(configs.y_arr)) / configs.n_threads;

    std::vector<int_calculator> calcs;

    for (size_t i = 0; i < configs.n_threads; i++, x_prev += x_step)
    {
        config new_configs = configs;

        new_configs.x_arr = std::make_tuple(x_prev, x_prev + x_step);
        // std::cout << "Prev  :  next " << x_prev << "  :  " << x_prev + x_step << "\n";
        // new_configs.y_arr = std::make_tuple(y_prev, y_prev + y_step);
        // x_prev += x_step;
        // y_prev += y_step;

        calcs.push_back(int_calculator(new_configs));
    }
    int_calculator::find_best_integral(configs);

    // std::vector<std::thread> threads;

    // auto start = get_current_time_fenced();

    // for (size_t i = 0; i < configs.n_threads; i++)
    // {
    //     threads.emplace_back(int_calculator::find_best_integral, std::ref(calcs[i]));
    // }
    // for (size_t i = 0; i < configs.n_threads; i++)
    // {
    //     threads[i].join();
    // }

    // std::cout << "Passed!\n";
    // double res = 0;
    // for (auto &x : calcs)
    // {
    //     std::cout << "Calculated " << x.result << "\n";
    //     res += x.result;
    // }

    // auto time_passed = get_current_time_fenced() - start;
    // std::cout << "Result is " << res << "\n";
    // std::cout << "Time is " << to_us(time_passed) << "\n";
    return 0;
}

/*

1. function x2 faster - done 
2. errors in reading
3. time - done
4. rel and abs error
5. size = size_t - done



 */