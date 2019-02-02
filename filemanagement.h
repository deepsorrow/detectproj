#ifndef FILEMANAGEMENT_H
#define FILEMANAGEMENT_H
#include <iostream>
#include <string>
#include <vector>
#include <dirent.h>
#include <opencv2/opencv.hpp>

static const int fps = 4;

class functions
{
public:
    static auto time_format(const int n) -> std::string;
    static auto to_time(const int counter, const int base_h,
                 const int base_m, const int base_s) -> std::string;
    static auto get_vector_of_files(const std::string path) -> std::vector<std::string>;
    static auto export_data_to_str(const int fnum,
                    const int base_h, const int base_m, const int base_s,
                    std::vector<std::vector<int>> cntsareas) -> std::string;
    static auto parse_txtframe(std::string &framestring, int &j_count, int &_base_h,
                               int &_base_m, int &_base_s,
                               std::vector<std::vector<int>> &_cntsareas) -> void;
};
#endif // FILEMANAGEMENT_H
