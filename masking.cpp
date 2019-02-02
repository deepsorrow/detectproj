#include "masking.h"

Mask::Mask(const cv::Mat &sample_frame, std::vector<cv::Point> &v)
{
    mask = cv::Mat::zeros(sample_frame.size(), sample_frame.type());
    vertices = v;

    const cv::Point* corner_list[1] = { &vertices[0] };

    int n_qnty = static_cast<int>(vertices.size());
    cv::fillPoly(mask, corner_list, &n_qnty, 1, cv::Scalar( 255, 255, 255 ), 8);

    int minx = 99999, miny = 99999, maxx = -1, maxy = -1;
    for(size_t i = 0; i < n_qnty; ++i)
    {
        minx = std::min(vertices[i].x, minx);
        miny = std::min(vertices[i].y, miny);
        maxx = std::max(vertices[i].x, maxx);
        maxy = std::max(vertices[i].y, maxy);
    }
    p1.x = minx;
    p1.y = miny;
    p2.x = maxx;
    p2.y = maxy;

    dstImage = cv::Mat::zeros(cv::Size(p2.x, p2.y), sample_frame.type());
    sample_frame(cv::Range(p1.y,p2.y), cv::Range(p1.x,p2.x))
            .copyTo(dstImage, mask(cv::Range(p1.y,p2.y), cv::Range(p1.x,p2.x)));

    cvtColor(dstImage, firstFrame, cv::COLOR_BGR2GRAY);
    GaussianBlur(firstFrame, firstFrame, cv::Size(21, 21), 0);
}

auto Mask::_findCountours(cv::Mat &frame) -> std::vector<std::vector<cv::Point>>
{
    cut_frame_using_mask(frame);
    cvtColor(dstImage, gray, cv::COLOR_BGR2GRAY);
    GaussianBlur(gray, gray, cv::Size(21, 21), 0);

    absdiff(firstFrame, gray, frameDelta);
    threshold(frameDelta, thresh, 25, 255, cv::THRESH_BINARY);
    dilate(thresh, thresh, cv::Mat(), cv::Point(-1,-1), 2);

    std::vector<std::vector<cv::Point>> cnts;
    findContours(thresh, cnts, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    return cnts;
}

auto Mask::_reset_diff_frame() -> void
{
    cvtColor(dstImage, firstFrame, cv::COLOR_BGR2GRAY);
    GaussianBlur(firstFrame, firstFrame, cv::Size(21, 21), 0);
}

auto Mask::cut_frame_using_mask(cv::Mat &frame) -> void
{
    dstImage = cv::Mat::zeros(cv::Size(p2.x, p2.y), frame.type());
    frame(cv::Range(p1.y,p2.y), cv::Range(p1.x,p2.x))
        .copyTo(dstImage, mask(cv::Range(p1.y,p2.y), cv::Range(p1.x,p2.x)));
}

auto Mask::set_detected(const int _timeout) -> void
{
    detected = true;
    timeout = _timeout;
}
