#include "dirent.h"
#include "filemanagement.h"

auto functions::export_data_to_str(const int fnum,
             const int base_h, const int base_m, const int base_s,
             std::vector<std::vector<int>> cntsareas) -> std::string
{
    // номер кадра и начальное время
    std::string s1 = std::to_string(fnum)+";"+std::to_string(base_h)+
                 ":"+std::to_string(base_m)+":"+std::to_string(base_s);

    // контуры в области масок
    std::string s2 = "[";
    for(size_t i=0; i < cntsareas.size(); ++i){
        s2 += "{";
        for(size_t k=0; k < cntsareas[i].size(); ++k)
            s2 += std::to_string(cntsareas[i][k])+";";
        s2 += "}";
    }
    s2 += "]\n";

    return (s1 + s2);
}

auto functions::time_format(const int n) -> std::string{
    std::string n_t = "";
    if(n <= 9)
        n_t = "0"+std::to_string(n);
    else
        n_t = std::to_string(n);
    return n_t;
}

auto functions::to_time(const int counter, const int base_h,
                        const int base_m, const int base_s) -> std::string {
    int cur_time_s = static_cast<int>(counter/fps)+base_s;
    int cur_time_m = cur_time_s/60+base_m;
    int cur_time_h = cur_time_m/60+base_h;
    cur_time_s = cur_time_s%60;
    cur_time_m = cur_time_m%60;

    return time_format(cur_time_h) + ":" + time_format(cur_time_m) + ":"
            + time_format(cur_time_s);
}

auto functions::get_vector_of_files(const std::string path) -> std::vector<std::string> {
    const char * path_c = path.c_str();
    std::vector<std::string> filenames;

    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir (path_c)) != NULL) {
      /* print all the files and directories within directory */
      while ((ent = readdir (dir)) != NULL) {
          std::string file = ent->d_name;
          if(file.length() > 3)
            filenames.push_back(path+ent->d_name);
          else
            std::cout << file << " was omitted!" << std::endl;
      }
      closedir (dir);
    }
    return filenames;
}

auto functions::parse_txtframe(std::string &framestring, int &j_count, int &_base_h,
                               int &_base_m, int &_base_s,
                               std::vector<std::vector<int>> &_cntsareas) -> void{
    size_t pos = 0;
    std::string token;
    if(framestring.empty())
        return;
    pos = framestring.find(';');
    j_count = std::stoi(framestring.substr(0, pos));
    framestring.erase(0, pos + 1);

    pos = framestring.find(':');
    _base_h = std::stoi(framestring.substr(0, pos));
    framestring.erase(0, pos + 1);

    pos = framestring.find(':');
    _base_m = std::stoi(framestring.substr(0, pos));
    framestring.erase(0, pos + 1);

    pos = framestring.find("[{");
    _base_s = std::stoi(framestring.substr(0, pos));
    framestring.erase(0, pos + 1 + 1);  // erase "[{"

    size_t pos1, pos2;
    for(size_t i=0; i<3; ++i){
        // [{.FIRST.}, {...}, {...}]
        while ((pos1 = framestring.find(';')) != std::string::npos &&
               (pos2 = framestring.find('}')) != std::string::npos &&
               pos1 < pos2)
        {
            int cnt = std::stoi(framestring.substr(0, pos1));
            framestring.erase(0, pos1 + 1); // erase ';'

            _cntsareas[i].push_back(cnt);
        }
        framestring.erase(0, 2); //erase '{'
    }
}


