#ifndef MASKING_H
#define MASKING_H
#include <vector>
#include <opencv2/opencv.hpp>

class Mask{
public:
    Mask ();
    Mask (const cv::Mat &sample_frame, std::vector<cv::Point> &v);

    auto _findCountours(cv::Mat &frame) -> std::vector<std::vector<cv::Point>>;
    auto _reset_diff_frame() -> void;
    auto _init_mask(cv::Mat &frame) -> void;
    auto set_detected(const int _timeout = 60) -> void;
    auto cut_frame_using_mask(cv::Mat &frame) -> void;

    cv::Mat gray, frameDelta, thresh, firstFrame, dstImage, mask;
    bool detected = false;
    int timeout = 0;
    cv::Point p1, p2; // to make rectangle, representing boundaries
    std::vector<cv::Point> vertices;
private:


};

#endif // MASKING_H
