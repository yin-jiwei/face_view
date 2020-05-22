//
// Created by yjw on 8/14/18.
//

#ifndef FACE_CFGDATA_H
#define FACE_CFGDATA_H

#include <string>

class FaceViewWindow;

using namespace std;

class CCfgData
{
public:
    virtual ~CCfgData();

    static CCfgData &Instance()
    {
        return instance_;
    }

    void Fetch();

    const string &get_full_path()
    {
        return full_path_;
    }

    const string &get_program_name()
    {
        return program_name_;
    }

    int get_local_server_port()
    {
        return local_server_port_;
    }

private:
    CCfgData();

public:
    FaceViewWindow *window_handle_ = nullptr;

private:
    static CCfgData instance_;

    string full_path_;
    string program_name_;

    string config_file_path_;

    int local_server_port_ = 9000;
};


#endif //FACE_CFGDATA_H
