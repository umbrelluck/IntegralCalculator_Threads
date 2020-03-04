#include <iostream>
#include <fstream>
#include <thread>

#include "integral/integral.h"

int main(int argc, char **argv)
{
    std::cout << "\n";
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

    for (size_t i = 0; i < configs.n_threads; i++)
    {
        config new_configs = configs;
        new_configs.x_arr = std::make_tuple(x_prev, x_prev + x_step);
        new_configs.y_arr = std::make_tuple(y_prev, y_prev + y_step);
        x_prev += x_step;
        y_prev += y_step;

        calcs.push_back(int_calculator(new_configs));
    }

    std::vector<std::thread> threads;
    for (size_t i = 0; i < configs.n_threads; i++)
    {
        //        threads.push_back(std::thread(hello, i));
        threads.emplace_back(int_calculator::find_best_integral, std::ref(calcs[i]));
    }
    for (size_t i = 0; i < configs.n_threads; i++)
    {
        threads[i].join();
    }

    double res = 0;
    for (auto &x : calcs)
    {
        std::cout << "Calculated " << x.result << "\n";
        res += x.result;
    }
    std::cout << "Result is " << res << "\n";
    return 0;
}

/*

1. function x2 faster - done 
2. errors in reading
3. time - done
4. rel and abs error
5. size = size_t - done



 */