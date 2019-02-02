#include "heatmapping.h"

auto Heatmap::init_heatmap() -> void{
    for(int i=0; i < heatmap_raw.size(); ++i){
        for(int j=0; j < heatmap_raw[0].size(); ++j){
            heatmap_raw[i][j] = 0;
        }
    }
}

auto Heatmap::update_heatmap_rawdata(cv::Mat &img) -> void{
    int cols = img.cols;
    int rows = img.rows;
    for(int i=0; i < rows; ++i){
        for(int j=0; j < cols; ++j){
            if(img.at<int>(i,j))
                heatmap_raw[j][i]++;
        }
    }
}

auto Heatmap::cvtRange(int rangemax) -> std::vector<std::vector<int>>
{
    int maxvalue = -1;

    //поиск максимального значения для определения коэфф. k
    for(size_t i=0; i<heatmap_raw.size(); ++i){
        for(size_t j=0; j<heatmap_raw[0].size(); ++j){
            maxvalue = std::max(maxvalue, heatmap_raw[i][j]);
        }
    }
    double k = maxvalue/rangemax;

    //перевод heatmap в заданную область значений
    std::vector<std::vector<int>> heatmap_new = heatmap_raw;
    for(size_t i=0; i<heatmap_raw.size(); ++i){
        for(size_t j=0; j<heatmap_raw[0].size(); ++j){
            heatmap_new[i][j] = round(static_cast<double>(heatmap_raw[i][j])/k);
        }
    }

    return heatmap_new;
}

auto Heatmap::heatmap_to_mat(const std::vector<std::vector<int>> &heatmap) -> cv::Mat{
    cv::Mat _img(cv::Size(heatmap.size(), heatmap[0].size()), CV_8UC1, cv::Scalar(0));

    int cols = _img.cols;
    int rows = _img.rows;
    for(int i=0; i<rows; ++i){
        for(int j=0; j<cols; ++j){
            _img.at<int>(i,j) = heatmap[j][i];
        }
    }
    return _img;
}

auto Heatmap::matIsEqual(const cv::Mat mat1, const cv::Mat mat2) -> bool{
    // treat two empty mat as identical as well
    if (mat1.empty() && mat2.empty()) {
        return true;
    }
    // if dimensionality of two mat is not identical, these two mat is not identical
    if (mat1.cols != mat2.cols || mat1.rows != mat2.rows || mat1.dims != mat2.dims) {
        return false;
    }
    cv::Mat diff;
    cv::compare(mat1, mat2, diff, cv::CMP_NE);
    int nz = cv::countNonZero(diff);
    return nz==0;
}
