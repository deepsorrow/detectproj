#ifndef DETECTION_H
#define DETECTION_H
#include "filemanagement.h"
#include "heatmapping.h"
#include "masking.h"
#include <string>
#include <vector>
#include <sstream>
#include <opencv2/opencv.hpp>

static const int FRAME_STEP = 1;

// структура datetime для хранения 'временных точек'
struct datetime
{
    datetime(){}
    datetime(const std::string &t){
        h = std::stoi(t.substr(0, 2));
        m = std::stoi(t.substr(3, 2));
        s = std::stoi(t.substr(6, 2));
        time = t;
    }
    datetime(const int &_h, const int &_m, const int &_s){
        int ss = _h*3600+_m*60+_s;
        time = to_time(ss, 0, 0, 0, 1);
        h = std::stoi(time.substr(0, 2));
        m = std::stoi(time.substr(3, 2));
        s = std::stoi(time.substr(6, 2));
    }
    auto time_format(const int n) -> std::string{
        std::string n_t = "";
        if(n <= 9)
            n_t = "0"+std::to_string(n);
        else
            n_t = std::to_string(n);
        return n_t;
    }
    auto to_time(const int counter, const int base_h,
                            const int base_m, const int base_s, const int _fps) -> std::string {
        int cur_time_s = static_cast<int>(counter/_fps)+base_s;
        int cur_time_m = cur_time_s/60+base_m;
        int cur_time_h = cur_time_m/60+base_h;
        cur_time_s = cur_time_s%60;
        cur_time_m = cur_time_m%60;

        return time_format(cur_time_h) + ":" + time_format(cur_time_m) + ":"
                + time_format(cur_time_s);
    }
    auto get_in_seconds() -> int { return (h*3600+m*60+s); }
    std::string time = "";
    int h=0, m=0, s=0;
    bool operator<(const datetime &dt) const{
        int s1 = h*3600+m*60+s;
        int s2 = dt.h*3600+dt.m*60+dt.s;
        if(s1 < s2)
            return true;
        else
            return false;
    }
    bool operator<=(const datetime &dt) const{
        int s1 = h*3600+m*60+s;
        int s2 = dt.h*3600+dt.m*60+dt.s;
        if(s1 <= s2)
            return true;
        else
            return false;
    }
    bool operator>(const datetime &dt) const{
        int s1 = h*3600+m*60+s;
        int s2 = dt.h*3600+dt.m*60+dt.s;
        if(s1 > s2)
            return true;
        else
            return false;
    }
    bool operator>=(const datetime &dt) const{
        int s1 = h*3600+m*60+s;
        int s2 = dt.h*3600+dt.m*60+dt.s;
        if(s1 >= s2)
            return true;
        else
            return false;
    }
    datetime operator-(const datetime &dt){
        int s1 = h*3600+m*60+s;
        int s2 = dt.h*3600+dt.m*60+dt.s;
        datetime res = datetime(to_time(s1-s2, 0, 0, 0, 1));
        return res;
    }
    datetime operator+(const datetime &dt){
        int s1 = h*3600+m*60+s;
        int s2 = dt.h*3600+dt.m*60+dt.s;
        datetime res = datetime(to_time(s2+s1, 0, 0, 0, 1));
        return res;
    }
    void operator=(const datetime &dt){
        h = dt.h;
        m = dt.m;
        s = dt.s;
        time = datetime(h,m,s).time;
    }
};


class Detector{
public:
    explicit Detector(const std::vector<std::string> vd,
                      const std::string shopnumber, const int indexcamera);
    auto get_summary() -> std::string;
    auto get_heatmap_rawdata() -> std::vector<std::vector<int>>;

    // without heatmap
    auto analyze_recording(const std::string &filename) -> void;
    auto get_sample_frame() -> cv::Mat;

    auto populate_verticies(const std::string shopnumber) -> void;
    auto set_basetime(const int h, const int m, const int s) -> void;
    auto save_debuginfo(const std::string &filename) -> void;
    auto analyze_from_txt(const std::string &filename) -> void;
    std::string debuginfo = "";
    int indexcamera = 0;

protected:
    auto add_vertex(const size_t vertex, cv::Point p) -> void;
    auto refresh_refframe() -> void;
    auto process_frame(cv::Mat &frame) -> void;
    auto process_txtframe(const int frame_n,
                          const std::vector<std::vector<int>> _cnts) -> void;
    auto draw_masks(cv::Mat &img, std::vector<cv::Point> &v) -> void;
    auto make_entry() -> void;
    auto save_masks_info() -> void;
    std::vector<std::string> videolist;
    std::vector<Mask> masks;
    std::vector<std::vector<cv::Point>> vv;
    //int k = 1;
    const size_t masks_qnty = 3;
    int base_h, base_m, base_s;
    int frame_n = 0;
    int frame_n_when_first_detected = 0;
    bool was_detection = false;
};

#endif // DETECTION_H
