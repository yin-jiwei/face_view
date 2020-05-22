//
// Created by yjw on 19-6-19.
//

#ifndef FACE_VIEW_FACEVIEWSERVER_H
#define FACE_VIEW_FACEVIEWSERVER_H

#include <thread>

class FaceViewServer
{
public:
    FaceViewServer(int port = 9000);

    virtual ~FaceViewServer();

private:
    void ThreadFunc();

    static void *ServeRequest(void *arg);

private:
    std::thread *thread_ = nullptr;
    bool shutdown_ = false;

    int port_ = 9000;
};

#endif //FACE_VIEW_FACEVIEWSERVER_H
