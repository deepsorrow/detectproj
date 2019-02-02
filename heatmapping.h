#ifndef HEATMAPPING_H
#define HEATMAPPING_H
#include <vector>
#include <opencv2/opencv.hpp>

class Heatmap
{
public:
    explicit Heatmap(const int cols, const int rows)
        : heatmap_raw(static_cast<size_t>(cols),
                      std::vector<int>(static_cast<size_t>(rows))) {}
    auto init_heatmap() -> void;
    auto update_heatmap_rawdata(cv::Mat &img) -> void;
    auto cvtRange(int rangemax=255) -> std::vector<std::vector<int>>;
    auto heatmap_to_mat(const std::vector<std::vector<int>> &heatmap) -> cv::Mat;
    auto matIsEqual(const cv::Mat mat1, const cv::Mat mat2) -> bool;

private:
    std::vector<std::vector<int>> heatmap_raw;
};

#endif // HEATMAPPING_H
