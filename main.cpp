#include "Logger.h"
#include "CfgData.h"
#include "FaceViewWindow.h"
#include "FaceViewServer.h"

int main()
{
    START_LOGGER
    string log_msg;

    INFO_LOG("..............................face_view start..............................")

    CCfgData::Instance().Fetch();

    FaceViewWindow face_view_window;

//    unique_ptr<FaceViewServer> face_view_server(new FaceViewServer(CCfgData::Instance().get_local_server_port()));
//    shared_ptr<FaceViewServer> face_view_server = make_shared<FaceViewServer>(
//            CCfgData::Instance().get_local_server_port());
    FaceViewServer face_view_server(CCfgData::Instance().get_local_server_port());

    face_view_window.MainLoop();

    INFO_LOG("..............................face_view exit...............................")
    STOP_LOGGER

    return 0;
}