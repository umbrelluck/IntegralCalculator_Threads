#include <atomic>
#include <chrono>
#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <tuple>
#include <string>
#include <cassert>
#include <sstream>

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
};

int readFromFile(std::ifstream &file, config &configs)
{
    std::string key, m_value;
    while (file >> key >> m_value)
    {
        if (key.find("abs") != std::string::npos)
            if (m_value == "=")
                file >> configs.abs_error;
            else
                configs.abs_error = std::stod(m_value);
        else if (key.find("rel") != std::string::npos)
            if (m_value == "=")
                file >> configs.rel_error;
            else
                configs.rel_error = std::stod(m_value);
        else if (key.find("n") != std::string::npos)
            if (m_value == "=")
                file >> configs.n_threads;
            else
                configs.n_threads = std::stoi(m_value);
        else if (key.find("size") != std::string::npos)
            if (m_value == "=")
                file >> configs.size;
            else
            {
                std::stringstream sstream(m_value);
                // configs.size = std::stoll(m_value);
                sstream >> configs.size;
            }
        else if (key.find("x") != std::string::npos)
            if (m_value == "=")
            {
                double x1, x2;
                file >> x1 >> x2;
                configs.x_arr = std::make_tuple(x1, x2);
            }
            else
            {
                double x2;
                file >> x2;
                configs.x_arr = std::make_tuple(std::stod(m_value), x2);
            }
        else if (key.find("y") != std::string::npos)
            if (m_value == "=")
            {
                double y1, y2;
                file >> y1 >> y2;
                configs.y_arr = std::make_tuple(y1, y2);
            }
            else
            {
                double y2;
                file >> y2;
                configs.y_arr = std::make_tuple(std::stod(m_value), y2);
            }
        else if (key.find("coeff") != std::string::npos)
        {
            int minus;
            if (m_value == "=")
                minus = 0;
            else
            {
                minus = 1;
                configs.coeff.push_back(std::stod(m_value));
            }
            double value;
            for (int i = 0; i < configs.size - minus; ++i)
            {
                file >> value;
                configs.coeff.push_back(value);
            }
        }
        else if (key.find("1") != std::string::npos)
        {
            int minus;
            if (m_value == "=")
                minus = 0;
            else
            {
                minus = 1;
                configs.base_val1.push_back(std::stod(m_value));
            }
            double value;
            for (int i = 0; i < configs.size - minus; ++i)
            {
                file >> value;
                configs.base_val1.push_back(value);
            }
        }
        else if (key.find("2") != std::string::npos)
        {
            int minus;
            if (m_value == "=")
                minus = 0;
            else
            {
                minus = 1;
                configs.base_val2.push_back(std::stod(m_value));
            }
            double value;
            for (int i = 0; i < configs.size - minus; ++i)
            {
                file >> value;
                configs.base_val2.push_back(value);
            }
        }
    }
    return 0;
}

int checkConfig(config &configs, int inst_check)
{
    if (inst_check)
        return inst_check;
    if (configs.abs_error < 0 || configs.rel_error < 0)
        return 1;
    return 0;
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
        std::cout << "    => Using default name 'conf.txt'\n";
        fileName = "conf.txt";
    }
    else if (argc > 2)
    {
        std::cout << "  ! Incorrect usage of the program (to many arguements)\n"
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
    int inst_check = readFromFile(file, configs);
    if (checkConfig(configs, inst_check))
    {
        std::cout << "  ! Error in configuration file\n";
        return 1;
    }

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