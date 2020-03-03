#include "config.h"

int config::readFromFile(std::ifstream &file)
{
    std::string key, m_value;
    while (file >> key >> m_value)
    {
        if (key.find("abs") != std::string::npos)
            if (m_value == "=")
                file >> this -> abs_error;
            else
                this -> abs_error = std::stod(m_value);
        else if (key.find("rel") != std::string::npos)
            if (m_value == "=")
                file >> this -> rel_error;
            else
                this -> rel_error = std::stod(m_value);
        else if (key.find("n") != std::string::npos)
            if (m_value == "=")
                file >> this -> n_threads;
            else
                this -> n_threads = std::stoi(m_value);
        else if (key.find("size") != std::string::npos)
            if (m_value == "=")
                file >> this -> size;
            else
            {
                std::stringstream sstream(m_value);
                // configs.size = std::stoll(m_value);
                sstream >> this -> size;
            }
        else if (key.find("x") != std::string::npos)
            if (m_value == "=")
            {
                double x1, x2;
                file >> x1 >> x2;
                this -> x_arr = std::make_tuple(x1, x2);
            }
            else
            {
                double x2;
                file >> x2;
                this -> x_arr = std::make_tuple(std::stod(m_value), x2);
            }
        else if (key.find("y") != std::string::npos)
            if (m_value == "=")
            {
                double y1, y2;
                file >> y1 >> y2;
                this -> y_arr = std::make_tuple(y1, y2);
            }
            else
            {
                double y2;
                file >> y2;
                this -> y_arr = std::make_tuple(std::stod(m_value), y2);
            }
        else if (key.find("coeff") != std::string::npos)
        {
            int minus;
            if (m_value == "=")
                minus = 0;
            else
            {
                minus = 1;
                this -> coeff.push_back(std::stod(m_value));
            }
            double value;
            for (int i = 0; i < this -> size - minus; ++i)
            {
                file >> value;
                this -> coeff.push_back(value);
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
                this -> base_val1.push_back(std::stod(m_value));
            }
            double value;
            for (int i = 0; i < this -> size - minus; ++i)
            {
                file >> value;
                this -> base_val1.push_back(value);
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
                this -> base_val2.push_back(std::stod(m_value));
            }
            double value;
            for (int i = 0; i < this -> size - minus; ++i)
            {
                file >> value;
                this -> base_val2.push_back(value);
            }
        }
    }
    return 0;
}

int config::check(int inst_check)
{
    if (inst_check)
        return inst_check;
    if (this -> abs_error < 0 || this -> rel_error < 0)
        return 1;
    return 0;
}