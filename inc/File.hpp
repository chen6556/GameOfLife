#pragma once
#include <fstream>
#include <vector>


namespace File
{
    std::vector<std::vector<bool>> read(const std::string &filename);

    void write(const std::string &filename, const std::vector<std::vector<bool>> &checkerboard);
};