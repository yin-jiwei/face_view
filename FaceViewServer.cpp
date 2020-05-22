//
// Created by yjw on 19-6-19.
//

#include <sstream>
#include "gsoap/json.h"
#include "gsoap/threads.h"
#include "Logger.h"
#include "Base64.h"
#include "FaceViewServer.h"
#include "CfgData.h"
#include "FaceViewWindow.h"

#define BACKLOG (100)    // Max. request backlog

using namespace std;

/* Don't need a namespace table. We put an empty one here to avoid link errors */
struct Namespace namespaces[] = {{nullptr, nullptr}};

FaceViewServer::FaceViewServer(int port)
{
    port_ = port;

    // start thread
    thread_ = new thread(&FaceViewServer::ThreadFunc, this);
}

FaceViewServer::~FaceViewServer()
{
    // quit thread
    shutdown_ = true;
    thread_->join();
    delete thread_;
}

void FaceViewServer::ThreadFunc()
{
    string log_message;

    log_message = "Thread FaceViewServer start...";
    INFO_LOG(log_message)

    soap *soap_ctx = soap_new1(SOAP_C_UTFSTRING);
    soap_ctx->accept_timeout = 10; // 10 sec
    soap_ctx->send_timeout = 10; // 10 sec
    soap_ctx->recv_timeout = 10; // 10 sec

    if (!soap_valid_socket(soap_bind(soap_ctx, nullptr, port_, BACKLOG)))
    {
        stringstream error_message;
        soap_stream_fault(soap_ctx, error_message);
        log_message.clear();
        log_message = "rest failure, errInfo:" + error_message.str();
        WARNING_LOG(log_message)
    }

    while (!shutdown_)
    {
        THREAD_TYPE tid;
        if (!soap_valid_socket(soap_accept(soap_ctx)))
        {
            /*stringstream error_message;
            soap_stream_fault(camera_restserver->soap_ctx_, error_message);
            log_message.clear();
            log_message = "rest failure, errInfo:" + error_message.str();

            RUNNING_LOGGER(log_message);
            WARNING_LOG(log_message);
            break;*/
        }
        else
        {
            THREAD_CREATE(&tid, ServeRequest, (void *) soap_copy(soap_ctx));
        }
    } // end while

    soap_destroy(soap_ctx);
    soap_end(soap_ctx);
    soap_free(soap_ctx);

    log_message.clear();
    log_message = "Thread FaceViewServer exit.";
    INFO_LOG(log_message)
}

void *FaceViewServer::ServeRequest(void *arg)
{
    THREAD_DETACH(THREAD_ID);

    string log_message;

    soap *ctx = (soap *) arg;
    value request(ctx);

    // HTTP keep-alive max number of iterations
    unsigned int k = ctx->max_keep_alive;

    do
    {
        if (ctx->max_keep_alive > 0 && !--k)
        {
            ctx->keep_alive = 0;
        }

        value response(ctx);

        // receive HTTP header (optional) and JSON content
        if (soap_begin_recv(ctx)
            || json_recv(ctx, request)
            || soap_end_recv(ctx))
        {
            soap_send_fault(ctx);
        }
        else
        {
            string request_path = ctx->path;

            if (request_path == "/faceview")
            {
                //auto face_info = new FaceInfo();
                auto face_info = new FaceInfo();
                face_info->person_id = (ULONG64) request["person_id"];
                face_info->pass_similarity = request["pass_similarity"];
                face_info->score = request["score"];
                face_info->photo_id = (char *) request["photo_id"];
                face_info->photo_name = (char *) request["photo_name"];
                face_info->photo_department = (char *) request["photo_department"];
                char *snapshot_base64 = request["snapshot_base64"];
                char *photo_base64 = request["photo_base64"];

                // base64 decode
                CBase64::Decode((unsigned char *) snapshot_base64, strlen(snapshot_base64),
                                (unsigned char *) face_info->snapshot_jpg, face_info->snapshot_size);

                CBase64::Decode((unsigned char *) photo_base64, strlen(photo_base64),
                                (unsigned char *) face_info->photo_jpg, face_info->photo_size);

                if (CCfgData::Instance().window_handle_ != nullptr)
                {
                    CCfgData::Instance().window_handle_->AddFaceInfo(face_info);
                }
            }

            // http content type
            ctx->http_content = "application/json; charset=utf-8";
            // http content length
            if (soap_begin_count(ctx)
                || ((ctx->mode & SOAP_IO_LENGTH) && json_send(ctx, response))
                || soap_end_count(ctx)
                || soap_response(ctx, SOAP_FILE)
                || json_send(ctx, response)
                || soap_end_send(ctx))
            {
                stringstream error_message;
                soap_stream_fault(ctx, error_message);
                log_message.clear();
                log_message = "rest failure, errInfo:" + error_message.str();
                WARNING_LOG(log_message)
            }
        }
        // close (keep-alive may keep socket open when client supports it)
        soap_closesock(ctx);

    } while (ctx->keep_alive);

    int err = ctx->error;

    // clean up
    soap_destroy(ctx);
    soap_end(ctx);
    soap_free(ctx);

    //pthread_exit((void *) err);
    pthread_exit(nullptr);
}