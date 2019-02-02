#include "detection.h"
#include "filemanagement.h"
#include <fstream>
#include <ctime>

using namespace std;

std::string cut_number_from_text(std::string &text, unsigned int &_offset_, int length){
    return text.substr(text.length()-_offset_, length);
}

//function to get previous date
void getPrevDate(int *day,int *month, int *year)
{
    //if there is first day of month
    if(*day==1)
    {
        //months which have 30 days in previous month
        if(*month==4|| *month==6|| *month==9|| *month==11)
        {
            *day=31;
            *month = *month -1;
        }
        //for MARCH, to define february last day
        else if(*month==3)
        {
            if(*year%4==0)
                *day=29;
            else
                *day=28;

            *month = *month -1;
        }
        //for January, to define December last day
        else if(*month==1)
        {
            *day=31;
            *month = 12;
            *year = *year - 1 ;
        }
        //for Feb, to define January last day
        else if(*month==2)
        {
            *day=31;
            *month = *month -1;
        }
        //for other months
        else
        {
            *day=30;
            *month = *month -1;
        }
    }
    //other days of month
    else
    {
        *day = *day-1;
    }

}

int main(int argc, char* argv[])
{   
    if(argc < 1){
        std::cout << "not enough arguments!!\n";
        return 0;
    }
    std::string shopnumber = argv[1];
    std::string cameraname = argv[2];
    int indexcamera = 0;
    if(argv[3])
        indexcamera = std::stoi(argv[3]);
    std::cout << indexcamera << "\n";

    std::string cashnumber = "";
    if (shopnumber == "A2")
        cashnumber = "3";
    else if(shopnumber == "A3")
        cashnumber = "1";
    else if(shopnumber == "A4")
        cashnumber = "2";
    else if(shopnumber == "A7")
        cashnumber = "2";
    else if(shopnumber == "A25" || shopnumber == "A25")
        cashnumber = "3";
    std::cout << "shopnumber == " << shopnumber << "; cashnumber == " << cashnumber << "\n";

    std::vector<std::string> hours = {"09", "10", "11", "12", "13", "14", "15", "16",
                                     "17", "18", "19"};

    std::string debuginfo;
    std::ofstream file;

    std::vector<std::string> fullvideolist;

    unsigned int _year_offset, _month_offset, _day_offset;
    // камеры Dlink
    if(shopnumber.substr(0, 3) != "A7" && shopnumber.substr(0,3) != "A25"){
        // 20181202_090453_6436.avi
        for(auto &hour : hours){
            std::string base_path = "\\\\217.138.143.1\\FTPChek\\videofiles\\"+shopnumber+"\\"
                    + cameraname +  "\\" + hour + "\\";

            std::vector<std::string> videolist = functions::get_vector_of_files(base_path);
            for(auto &videopath : videolist)
                fullvideolist.push_back(videopath);
        }
        _year_offset = 24;
        _month_offset = 20;
        _day_offset = 18;
    }
    // камеры HiWatch
    else{
        // 20181201090000_20181201090856_1.mp4
            std::string shopnumber_folder = shopnumber.substr(0, 3);

            std::string base_path = "\\\\217.138.143.1\\FTPChek\\videofiles\\"+
                    shopnumber_folder+"\\"+cameraname+"\\";
            std::cout << "base_path = " << base_path << "\n";
            fullvideolist = functions::get_vector_of_files(base_path);

            _year_offset = 35;
            _month_offset = 31;
            _day_offset = 29;
    }

    std::cout << "Received the following files:" << std::endl;
    for(auto file : fullvideolist)
        std::cout << file << std::endl;
    std::cout << std::endl;

    Detector d1(fullvideolist, shopnumber, indexcamera);

    std::string videoname_example = fullvideolist[0];
    std::string year = cut_number_from_text(videoname_example, _year_offset, 4);
    std::string month = functions::time_format(std::stoi(cut_number_from_text(videoname_example, _month_offset, 2)));
    std::string day = functions::time_format(std::stoi(cut_number_from_text(videoname_example, _day_offset, 2)));

    std::cout << "year = " << year << "; month = " << month << "; day= " << day;

    if(indexcamera != 0)
        d1.analyze_recording("C:\\inifiles\\"
      + shopnumber + "_" + day + month + year + "_" + cashnumber + "_" + std::to_string(indexcamera) + ".ini");
    else
        d1.analyze_recording("C:\\inifiles\\"
      + shopnumber + "_" + day + month + year + "_" + cashnumber + ".ini");

    return 0;
}
