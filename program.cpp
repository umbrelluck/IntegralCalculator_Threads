#include <iostream>
#include <iostream>
#include <fstream>
#include <vector>
#include <math.h>
#include <map>

double function(double x1, double x2, int m, std::vector<double> &a1, std::vector<double> &a2, std::vector<double> &c)
{
    double sum = 0;
    for (auto i = 0; i < m; ++i)
    {
        sum += c[i] * exp(-(1 / M_PI) * (pow((x1 - a1[i]), 2) - pow((x2 - a2[i]), 2))) * cos(M_PI * (pow((x1 - a1[i]), 2) - pow((x2 - a2[i]), 2)));
    }
    return -sum;
}

void calculateIntegral(std::map<std::string, float> &config, std::vector<double> &a1, std::vector<double> &a2, std::vector<double> &c)
{
}

void readFromFile(std::ifstream &file, std::map<std::string, float> *config, std::vector<double> *a1, std::vector<double> *a2, std::vector<double> *c)
{
    std::string key, value;
    while (file >> value)
    {
        if (!std::isdigit(value[0]))
            key = value;
        else
            float dp = std::stof(value);
        // config.
        // file >> value;
        // config.insert({key, value});
    }
}

int checkConfig(std::map<std::string, float> config)
{
    return 1;
}

int main(int argc, char **argv)
{
    if (argc == 1)
    {
        std::cout << "  ! No file specified\n";
        return 1;
    }
    std::ifstream file(argv[1]);
    if (!file)
    {
        std::cout << "  ! No such file\n";
        return 1;
    }
    std::map<std::string, float> config = {
        {"abs", 0},
        {"rel", 0},
        {"n_threads", 0},
        {"x1", 0},
        {"x2", 0},
        {"y1", 0},
        {"size", 0},
        {"abs", 0},
    };
    std::vector<double> a1, a2, c;
    readFromFile(file, &config, &a1, &a2, &c);
    if (checkConfig(config))
        std::cout << "  ! Error in configuration file\n";
    return 1;
    return 0;
}