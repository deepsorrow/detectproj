#include "detection.h"

using namespace std;

Detector::Detector(const std::vector<std::string> vd, const std::string shopnumber, const int indexcamera = 1)
{
    videolist = vd;
    this->indexcamera = indexcamera;
    populate_verticies(shopnumber);
    for(size_t i=0; i<masks_qnty; ++i)
        masks.push_back(Mask(get_sample_frame(), vv[i]));
}

auto Detector::draw_masks(cv::Mat &img, std::vector<cv::Point> &v) -> void{
    for(size_t i=1; i<v.size(); ++i){
        cv::line(img, v[i-1], v[i], cv::Scalar(0,0,255), 2);
    }
    cv::line(img, v[v.size()-1], v[0], cv::Scalar(0,0,255), 2);
}

auto Detector::save_debuginfo(const std::string &filename) -> void{
    std::ofstream file;
    file.open(filename);
    file << debuginfo;
    file.close();
}

auto Detector::get_sample_frame() -> cv::Mat{
    cv::Mat first_frame;
    cv::VideoCapture camera(videolist[0]);
    camera.read(first_frame);

    std::cout << "first frame was read.\n";
    return first_frame;
}

auto Detector::refresh_refframe() -> void{
    if(frame_n % FRAME_STEP*2 == 0){
        for(size_t i=0; i<masks_qnty; ++i)
            masks[i]._reset_diff_frame();

        if(frame_n % (fps*60) == 0) // each 60 seconds print current stage
            std::cout << functions::to_time(frame_n, base_h, base_m, base_s) << std::endl;
    }
}

auto Detector::process_frame(cv::Mat &frame) -> void{
    std::vector<std::vector<std::vector<cv::Point>>> cnts(masks_qnty);
    for(size_t i=0; i<masks_qnty; ++i)
        cnts[i] = masks[i]._findCountours(frame);

    std::vector<std::vector<int>> cntsareas(masks_qnty);

    for(size_t i = 0; i < masks_qnty; ++i){
        for(size_t j = 0; j < cnts[i].size(); j++) {
            int _contourArea = static_cast<int>(contourArea(cnts[i][j])); // обрисовать область,
            // в котором было движение
            cntsareas[i].push_back(_contourArea);
            //монетница (движение около монетницы редки, поэтому там таймер ожидания больше)
        }

    }

    debuginfo += functions::export_data_to_str(frame_n, base_h, base_m, base_s, cntsareas);

    frame_n+=FRAME_STEP;
}

auto Detector::analyze_recording(const std::string &filename) -> void{
    /* The main function is dedicated to compute and generate motion&masks data and save it to txt file.
     * Created txt file should be then analyzed using Detector::analyze_from_txt function.
    */
    save_masks_info();

    std::ofstream file;
    file.open(filename);

    debuginfo+="-----BEGIN MOTION DATA BLOCK-----\n";

    cv::Mat frame;
    for(std::string &videopath : videolist){
        std::cout << "vid:" << videopath.substr(videopath.length()-12, 2);
        if(videopath.substr(videopath.length()-3, 3) == "mp4"){
            // 20181201090000_20181201090856_1.mp4
            datetime temp(std::stoi(videopath.substr(videopath.length()-27,2)),
                          std::stoi(videopath.substr(videopath.length()-25,2)),
                          std::stoi(videopath.substr(videopath.length()-23,2)));
            temp = temp - datetime(0,0,9);
            std::cout << "temp.h=" << temp.h << ";temp.m=" << temp.m << ";temp.s=" << temp.s;
            set_basetime(temp.h, temp.m, temp.s);
        }
        else{
            // 20181202_090453_6436.avi
            set_basetime(std::stoi(videopath.substr(videopath.length()-15,2)), //h
                         std::stoi(videopath.substr(videopath.length()-13,2)), //m
                         std::stoi(videopath.substr(videopath.length()-11,2))); //s
        }

        frame_n = 0;
        cv::VideoCapture camera(videopath);

        std::cout << videopath << " has just began processing..." << std::endl;

        clock_t _begin = clock();
        while(camera.read(frame))
        {
            refresh_refframe(); // update reference frame if needed
            process_frame(frame); // compare ref. frame with current frame to get result in debuginfo
            //camera.set(cv::CAP_PROP_POS_FRAMES, frame_pos+FRAME_STEP-1);
            //frame_pos += FRAME_STEP;
        }
        clock_t _end = clock();
        double elapsed_time = double(_end - _begin) / CLOCKS_PER_SEC;
        file << debuginfo; // output result of analysis right away
        debuginfo = "";

        std::cout << videopath << " has been processed! Time elapsed: " << elapsed_time << "s"
                  << "\n";
    }
    debuginfo += "-----END OF MOTION DATA BLOCK-----\n";
    file.close();
}

auto Detector::make_entry() -> void{
    /*
     * Makes entry of the motion.
    */
    std::string s_from = functions::to_time(frame_n_when_first_detected,base_h,base_m,base_s)+";";
    std::string s_to = functions::to_time(frame_n-60,base_h,base_m,base_s)+";";
    std::cout << s_from + "-" + s_to + " an entry was added. w/ " << std::endl;
}

auto Detector::add_vertex(const size_t vertex, cv::Point p) -> void{
    vv[vertex].push_back(p);
}

auto Detector::populate_verticies(const std::string shopnumber) -> void{
    for(size_t i = 0; i < masks_qnty; ++i)
        vv.push_back(std::vector<cv::Point>());

    std::cout << "indexcamera " << indexcamera << " is selected!\n";
    // 0 - зона покупателя, 1 - продавца, 2 - монетницы
    if(shopnumber == "A2"){
    //покупатель
    add_vertex(0, cv::Point(522, 796));
    add_vertex(0, cv::Point(537, 608));
    add_vertex(0, cv::Point(716, 583));
    add_vertex(0, cv::Point(733, 794));
    //продавец
    add_vertex(1, cv::Point(914, 797));
    add_vertex(1, cv::Point(838, 499));
    add_vertex(1, cv::Point(870, 499));
    add_vertex(1, cv::Point(879, 428));
    add_vertex(1, cv::Point(991, 432));
    add_vertex(1, cv::Point(985, 514));
    add_vertex(1, cv::Point(1006, 520));
    add_vertex(1, cv::Point(1011, 482));
    add_vertex(1, cv::Point(1059, 482));
    add_vertex(1, cv::Point(1067, 508));
    add_vertex(1, cv::Point(1116, 521));
    add_vertex(1, cv::Point(1279, 392));
    add_vertex(1, cv::Point(1279, 797));
    //монетница
    add_vertex(2, cv::Point(755, 758));
    add_vertex(2, cv::Point(725, 605));
    add_vertex(2, cv::Point(769, 481));
    add_vertex(2, cv::Point(820, 486));
    add_vertex(2, cv::Point(869, 759));
    }

    //покупатель
    else if(shopnumber == "A3"){
    add_vertex(0, cv::Point(639 , 778 ));
    add_vertex(0, cv::Point(412 , 757 ));
    add_vertex(0, cv::Point(424 , 605 ));
    add_vertex(0, cv::Point(645 , 528 ));
    //продавец
    add_vertex(1, cv::Point(981 , 793 ));
    add_vertex(1, cv::Point(909 , 567 ));
    add_vertex(1, cv::Point(791 , 527 ));
    add_vertex(1, cv::Point(824 , 404 ));
    add_vertex(1, cv::Point(882 , 405 ));
    add_vertex(1, cv::Point(907 , 518 ));
    add_vertex(1, cv::Point(933 , 622 ));
    add_vertex(1, cv::Point(1279 , 652 ));
    add_vertex(1, cv::Point(1279 , 798 ));
    //монетница
    add_vertex(2, cv::Point(766 , 645 ));
    add_vertex(2, cv::Point(698 , 645 ));
    add_vertex(2, cv::Point(697 , 592 ));
    add_vertex(2, cv::Point(865 , 595 ));
    add_vertex(2, cv::Point(868 , 638 ));
    }
    else if(shopnumber == "A4"){
    //покупатель
    add_vertex(0, cv::Point(529, 718));
    add_vertex(0, cv::Point(0, 715));
    add_vertex(0, cv::Point(0, 337));
    add_vertex(0, cv::Point(540, 504));
    //продавец
    add_vertex(1, cv::Point(960, 705));
    add_vertex(1, cv::Point(849, 460));
    add_vertex(1, cv::Point(828, 422));
    add_vertex(1, cv::Point(734, 405));
    add_vertex(1, cv::Point(698, 338));
    add_vertex(1, cv::Point(668, 199));
    add_vertex(1, cv::Point(740, 150));
    add_vertex(1, cv::Point(773, 319));
    add_vertex(1, cv::Point(827, 330));
    add_vertex(1, cv::Point(842, 422));
    add_vertex(1, cv::Point(1259, 252));
    add_vertex(1, cv::Point(1274, 711));
    //монетница
    add_vertex(2, cv::Point(660, 697));
    add_vertex(2, cv::Point(558, 563));
    add_vertex(2, cv::Point(790, 548));
    add_vertex(2, cv::Point(836, 696));
    }

    else if(shopnumber == "A7"){
    //покупатель
    add_vertex(0, cv::Point(585, 677));
    add_vertex(0, cv::Point(418, 634));
    add_vertex(0, cv::Point(455, 469));
    add_vertex(0, cv::Point(632, 525));
    //продавец
    add_vertex(1, cv::Point(876, 710));
    add_vertex(1, cv::Point(860, 437));
    add_vertex(1, cv::Point(1067, 431));
    add_vertex(1, cv::Point(1098, 713));
    //монетница
    add_vertex(2, cv::Point(681, 521));
    add_vertex(2, cv::Point(633, 614));
    add_vertex(2, cv::Point(765, 618));
    add_vertex(2, cv::Point(784, 529));
    }

    else if(shopnumber == "A25" && indexcamera == 1){
    //покупатель
    add_vertex(0, cv::Point(578, 710));
    add_vertex(0, cv::Point(572, 539));
    add_vertex(0, cv::Point(752, 376));
    add_vertex(0, cv::Point(867, 706));
    //продавец
    add_vertex(1, cv::Point(248, 715));
    add_vertex(1, cv::Point(10, 711));
    add_vertex(1, cv::Point(70, 514));
    add_vertex(1, cv::Point(291, 591));
    //монетница
    add_vertex(2, cv::Point(526, 599));
    add_vertex(2, cv::Point(531, 682));
    add_vertex(2, cv::Point(461, 685));
    add_vertex(2, cv::Point(407, 587));
    }

    else if(shopnumber == "A25" && indexcamera == 2){
    //покупатель
    add_vertex(0, cv::Point(559, 301));
    add_vertex(0, cv::Point(553, 167));
    add_vertex(0, cv::Point(691, 174));
    add_vertex(0, cv::Point(696, 303));
    //продавец
    add_vertex(1, cv::Point(150, 336));
    add_vertex(1, cv::Point(338, 328));
    add_vertex(1, cv::Point(306, 202));
    add_vertex(1, cv::Point(145, 185));
    //монетница
    add_vertex(2, cv::Point(428, 230));
    add_vertex(2, cv::Point(426, 207));
    add_vertex(2, cv::Point(514, 211));
    add_vertex(2, cv::Point(510, 237));
    }

    else if(shopnumber == "A25" && indexcamera == 3){
    //покупатель
    add_vertex(0, cv::Point(794, 366));
    add_vertex(0, cv::Point(641, 377));
    add_vertex(0, cv::Point(662, 648));
    add_vertex(0, cv::Point(818, 636));
    //продавец
    add_vertex(1, cv::Point(1031, 565));
    add_vertex(1, cv::Point(977, 454));
    add_vertex(1, cv::Point(1161, 414));
    add_vertex(1, cv::Point(1197, 582));
    //монетница
    add_vertex(2, cv::Point(832, 495));
    add_vertex(2, cv::Point(821, 428));
    add_vertex(2, cv::Point(935, 438));
    add_vertex(2, cv::Point(941, 492));
    }

    else if(shopnumber == "A28"){
    //покупатель
    add_vertex(0, cv::Point(794, 366));
    add_vertex(0, cv::Point(641, 377));
    add_vertex(0, cv::Point(662, 648));
    add_vertex(0, cv::Point(818, 636));
    //продавец
    add_vertex(1, cv::Point(1031, 565));
    add_vertex(1, cv::Point(977, 454));
    add_vertex(1, cv::Point(1161, 414));
    add_vertex(1, cv::Point(1197, 582));
    //монетница
    add_vertex(2, cv::Point(832, 495));
    add_vertex(2, cv::Point(821, 428));
    add_vertex(2, cv::Point(935, 438));
    add_vertex(2, cv::Point(941, 492));
    }

}

auto Detector::set_basetime(const int h, const int m, const int s) -> void {
    base_h = h;
    base_m = m;
    base_s = s;
    std::cout << "base_h = " << base_h << "; base_m = " << base_m << "; base_s = " << base_s << "\n";
}

auto Detector::save_masks_info() -> void{
    debuginfo+="-----BEGIN MASKS VERTICES BLOCK-----\n";
    for(size_t i = 0; i < masks_qnty; ++i){
        for(size_t j = 0; j < vv[i].size(); ++j){
            debuginfo+=std::to_string(i)+";"
                    +std::to_string(vv[i][j].x)+";"+std::to_string(vv[i][j].y)+"\n";
        }
    }
    debuginfo+="-----END OF MASKS VERTICES BLOCK-----\n";
}
