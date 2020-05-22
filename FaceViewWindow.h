//
// Created by yjw on 19-6-20.
//

#ifndef FACE_VIEW_FACEVIEWWINDOW_H
#define FACE_VIEW_FACEVIEWWINDOW_H

#include <string>
#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include "ThreadSafeArray.h"

#define BMP_SIZE                        (1920*1200*3 + 100)

typedef struct FaceInfo
{
    unsigned long person_id = 0;  /// camera person id
    char *snapshot_jpg = nullptr;
    int snapshot_size = 0;

    int pass_similarity = 0;
    int score = 0;

    string photo_id;
    string photo_name;
    string photo_department;
    char *photo_jpg = nullptr;
    int photo_size = 0;

    // SDL_Texture
    SDL_Texture *unit_texture = nullptr;
    SDL_Texture *snapshot_texture = nullptr;
    SDL_Texture *photo_texture = nullptr;
    SDL_Texture *id_texture = nullptr;
    SDL_Texture *name_texture = nullptr;
    SDL_Texture *department_texture = nullptr;
    SDL_Texture *score_texture = nullptr;

    FaceInfo()
    {
        snapshot_jpg = new char[BMP_SIZE];
        memset(snapshot_jpg, 0, BMP_SIZE);
        snapshot_size = 0;

        photo_jpg = new char[BMP_SIZE];
        memset(photo_jpg, 0, BMP_SIZE);
        photo_size = 0;
    }

    ~FaceInfo()
    {
        if (nullptr != snapshot_jpg)
        {
            delete[] snapshot_jpg;
            snapshot_jpg = nullptr;
        }

        if (nullptr != photo_jpg)
        {
            delete[] photo_jpg;
            photo_jpg = nullptr;
        }

        if (nullptr != unit_texture)
        {
            SDL_DestroyTexture(unit_texture);
            unit_texture = nullptr;
        }

        if (nullptr != snapshot_texture)
        {
            SDL_DestroyTexture(snapshot_texture);
            snapshot_texture = nullptr;
        }

        if (nullptr != photo_texture)
        {
            SDL_DestroyTexture(photo_texture);
            photo_texture = nullptr;
        }

        if (nullptr != id_texture)
        {
            SDL_DestroyTexture(id_texture);
            id_texture = nullptr;
        }

        if (nullptr != name_texture)
        {
            SDL_DestroyTexture(name_texture);
            name_texture = nullptr;
        }

        if (nullptr != department_texture)
        {
            SDL_DestroyTexture(department_texture);
            department_texture = nullptr;
        }

        if (nullptr != score_texture)
        {
            SDL_DestroyTexture(score_texture);
            score_texture = nullptr;
        }
    }
} *PFaceInfo;

class FaceViewWindow
{
public:
    FaceViewWindow();

    virtual ~FaceViewWindow();

    int MainLoop();

    void AddFaceInfo(PFaceInfo face_info);

private:
    void RenderFace();

    //common
    void RenderTexture(SDL_Texture *tex, int x, int y, SDL_Rect *clip = nullptr);

    void RenderTexture(SDL_Texture *tex, SDL_Rect dst, SDL_Rect *clip = nullptr);

    TTF_Font *GetFont(const string &font_file, int font_size);

    SDL_Texture *GetTextTexture(TTF_Font *font, const string &message, SDL_Color color, Uint32 wrap_length = 0);

    SDL_Texture *LoadTexture(const string &image_file);

    Mix_Music *LoadMusic(const string &music_file);

    void SetRenderBlack();

private:
    CThreadSafeArray<PFaceInfo> face_info_list_;
    int face_list_maxsize_ = 12;
    int row_number_ = 4;
    int row_height_ = 231;
    int column_width_ = 623;

    bool quit_ = false;
    SDL_Window *sdl_window_ = nullptr;
    SDL_Renderer *sdl_renderer_ = nullptr;

    TTF_Font *ttf_font24_ = nullptr;
    TTF_Font *ttf_font16_ = nullptr;

    Mix_Music *music_ok_ = nullptr;
    Mix_Music *music_confirm_ = nullptr;

    SDL_Texture *background_texture_ = nullptr;
    SDL_Texture *unit_ok_texture_ = nullptr;
    SDL_Texture *unit_confirm_texture_ = nullptr;

};

#endif //FACE_VIEW_FACEVIEWWINDOW_H
