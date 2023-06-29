#include "File.hpp"
#include <string>


std::vector<std::vector<bool>> File::read(const std::string &filename)
{
    std::vector<std::vector<bool>> result;
    std::ifstream fs(filename);
    char ch, cache[4];
    int row, col, index = 0;

    while (fs.get(ch))
    {
        if (ch >= '0' && ch <= '9')
        {
            cache[index++] = ch;
        }
        else if (ch == ',')
        {
            cache[index] = '\0';
            index = 0;
            row = std::atoi(cache);
        }
        else if (ch == '\n')
        {
            cache[index] = '\0';
            index = 0;
            col = std::atoi(cache);
            break;
        }
    }
    result.assign(row, std::vector<bool>(col, false));

    while (fs.get(ch))
    {
        if (ch >= '0' && ch <= '9')
        {
            cache[index++] = ch;
        }
        else if (ch == ',')
        {
            cache[index] = '\0';
            index = 0;
            row = std::atoi(cache);
        }
        else if (ch == '\n')
        {
            cache[index] = '\0';
            index = 0;
            col = std::atoi(cache);
            result[row][col] = true;
        }
    }

    fs.close();
    return result;
}


void File::write(const std::string &filename, const std::vector<std::vector<bool>> &checkerboard)
{
    std::ofstream fs;
    fs.open(filename);

    const int rows = checkerboard.size(), cols = checkerboard.back().size();
    fs.write(std::to_string(rows).append(",").append(std::to_string(cols)).c_str(),
        std::to_string(rows).append(",").append(std::to_string(cols)).size());
    fs.put('\n');

    for (int i = 0; i < rows; ++i)
    {
        for (int j = 0; j < cols; ++j)
        {
            if (checkerboard[i][j])
            {
                fs.write(std::to_string(i).append(",").append(std::to_string(j)).c_str(),
                    std::to_string(i).append(",").append(std::to_string(j)).size());
                fs.put('\n');
            }
        }
    }

    fs.close();
}