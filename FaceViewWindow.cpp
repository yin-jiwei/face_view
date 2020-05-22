//
// Created by yjw on 19-6-20.
//

#include "FaceViewWindow.h"
#include "CfgData.h"
#include "Logger.h"

FaceViewWindow::FaceViewWindow()
{
    string log_msg;

    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
    {
        log_msg = "SDL_Init error: ";
        log_msg += SDL_GetError();
        SEVERE_LOG(log_msg);
    }

    if (IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG) <= 0)
    {
        log_msg = "IMG_Init error: ";
        log_msg += SDL_GetError();
        SEVERE_LOG(log_msg);
    }

    if (TTF_Init() != 0)
    {
        log_msg = "TTF_Init error: ";
        log_msg += SDL_GetError();
        SEVERE_LOG(log_msg);
    }

    if (Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 2, 1024) != 0)
    {
        log_msg = "Mix_OpenAudio error: ";
        log_msg += SDL_GetError();
        SEVERE_LOG(log_msg);
    }

    sdl_window_ = SDL_CreateWindow("FaceView", 0, 0, 1920, 1080, SDL_WINDOW_FULLSCREEN_DESKTOP);
    if (sdl_window_ == nullptr)
    {
        log_msg = "SDL_CreateWindow error: ";
        log_msg += SDL_GetError();
        SEVERE_LOG(log_msg);
    }

    sdl_renderer_ = SDL_CreateRenderer(sdl_window_, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (sdl_renderer_ == nullptr)
    {
        log_msg = "SDL_CreateRenderer error: ";
        log_msg += SDL_GetError();
        SEVERE_LOG(log_msg);
    }

    int window_width = 0;
    int window_height = 0;
    int columu_number = 0;
    SDL_GetWindowSize(sdl_window_, &window_width, &window_height);

    row_number_ = window_height / row_height_;
    columu_number = window_width / column_width_;
    face_list_maxsize_ = row_number_ * columu_number;

    string font_file = CCfgData::Instance().get_full_path() + "res/font.ttf";
    ttf_font24_ = GetFont(font_file, 24);
    ttf_font16_ = GetFont(font_file, 16);
    SDL_SetRenderDrawBlendMode(sdl_renderer_, SDL_BLENDMODE_BLEND);
//    SetRenderBlue();

    string music_ok_file = CCfgData::Instance().get_full_path() + "res/ok.wav";
    music_ok_ = LoadMusic(music_ok_file);

    string music_confirm_file = CCfgData::Instance().get_full_path() + "res/confirm.wav";
    music_confirm_ = LoadMusic(music_confirm_file);

    string background_file = CCfgData::Instance().get_full_path() + "res/bg.png";
    background_texture_ = LoadTexture(background_file);

    string unit_ok_file = CCfgData::Instance().get_full_path() + "res/unit_ok.png";
    unit_ok_texture_ = LoadTexture(unit_ok_file);

    string unit_confirm_file = CCfgData::Instance().get_full_path() + "res/unit_confirm.png";
    unit_confirm_texture_ = LoadTexture(unit_confirm_file);

    CCfgData::Instance().window_handle_ = this;
}

FaceViewWindow::~FaceViewWindow()
{
    CCfgData::Instance().window_handle_ = nullptr;

    face_info_list_.Lock();
    while (face_info_list_.GetSize() > 0)
    {
        delete face_info_list_.RemoveHead();
    }
    face_info_list_.Unlock();

    if (nullptr != unit_confirm_texture_)
    {
        SDL_DestroyTexture(unit_confirm_texture_);
        unit_confirm_texture_ = nullptr;
    }

    if (nullptr != unit_ok_texture_)
    {
        SDL_DestroyTexture(unit_ok_texture_);
        unit_ok_texture_ = nullptr;
    }

    if (nullptr != background_texture_)
    {
        SDL_DestroyTexture(background_texture_);
        background_texture_ = nullptr;
    }

    if (music_confirm_ != nullptr)
    {
        Mix_FreeMusic(music_confirm_);
        music_confirm_ = nullptr;
    }

    if (music_ok_ != nullptr)
    {
        Mix_FreeMusic(music_ok_);
        music_ok_ = nullptr;
    }

    if (ttf_font16_ != nullptr)
    {
        TTF_CloseFont(ttf_font16_);
        ttf_font16_ = nullptr;
    }

    if (ttf_font24_ != nullptr)
    {
        TTF_CloseFont(ttf_font24_);
        ttf_font24_ = nullptr;
    }

    if (sdl_renderer_ != nullptr)
    {
        SDL_DestroyRenderer(sdl_renderer_);
        sdl_renderer_ = nullptr;
    }

    if (sdl_window_ != nullptr)
    {
        SDL_DestroyWindow(sdl_window_);
        sdl_window_ = nullptr;
    }

    Mix_CloseAudio();
    Mix_Quit();
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
}

int FaceViewWindow::MainLoop()
{
    INFO_LOG("PollEvent start...")

    SDL_Event event;
    while (!quit_)
    {
        //Event Polling
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                quit_ = true;
            }
            if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)
            {
                quit_ = true;
            }
        }

        SetRenderBlack();
        SDL_RenderClear(sdl_renderer_);

        RenderFace();

        SDL_RenderPresent(sdl_renderer_);
    }

    INFO_LOG("PollEvent end")
    return 0;
}

void FaceViewWindow::RenderFace()
{
    // background
    RenderTexture(background_texture_, 0, 0);

    face_info_list_.Lock();
    if (face_info_list_.GetSize() > face_list_maxsize_)
    {
        delete face_info_list_.RemoveHead();
    }

    for (int i = 0; i < face_list_maxsize_; ++i)
    {
        PFaceInfo face_info = nullptr;

        if (face_info_list_.GetSize() <= i)
        {
            break;
        }

        int nSize = face_info_list_.GetSize() < face_list_maxsize_ ? face_info_list_.GetSize() : face_list_maxsize_;
        face_info = face_info_list_.GetAt(nSize - 1 - i);

//        //unit
//        if (face_info->unit_texture == nullptr)
//        {
//            face_info->unit_texture = SDL_CreateTexture(sdl_renderer_,
//                                                        SDL_PIXELFORMAT_RGBA8888,
//                                                        SDL_TEXTUREACCESS_TARGET,
//                                                        600,
//                                                        231);
//
//            SDL_SetRenderTarget(sdl_renderer_, face_info->unit_texture);
//            SetRenderBlack();
//            SDL_RenderClear(sdl_renderer_);
//
//            // unit background
//            if (face_info->score < face_info->pass_similarity)
//            {
//                RenderTexture(unit_confirm_texture_, 0, 0);
//            }
//            else
//            {
//                RenderTexture(unit_ok_texture_, 0, 0);
//            }
//
//            // photo
//            if (face_info->photo_texture == nullptr)
//            {
//                SDL_RWops *rwops_src = SDL_RWFromMem(face_info->photo_jpg, face_info->photo_size);
//                if (rwops_src != nullptr)
//                {
//                    face_info->photo_texture = IMG_LoadTexture_RW(sdl_renderer_, rwops_src, 1);
//                }
//            }
//
//            SDL_Rect dst_rect;
//            dst_rect.x = 45;
//            dst_rect.y = 42;
//            dst_rect.w = 106;
//            dst_rect.h = 149;
//
//            RenderTexture(face_info->photo_texture, dst_rect);
//
//            // snapshot
//            if (face_info->snapshot_texture == nullptr)
//            {
//                SDL_RWops *rwops_src = SDL_RWFromMem(face_info->snapshot_jpg, face_info->snapshot_size);
//                if (rwops_src != nullptr)
//                {
//                    face_info->snapshot_texture = IMG_LoadTexture_RW(sdl_renderer_, rwops_src, 1);
//                }
//            }
//
//            dst_rect.x = 171;
//            dst_rect.y = 42;
//            dst_rect.w = 106;
//            dst_rect.h = 149;
//            RenderTexture(face_info->snapshot_texture, dst_rect);
//
//            // name
//            if (face_info->name_texture == nullptr)
//            {
//                SDL_Color sdl_color = {208, 248, 255, 255};
//
//                face_info->name_texture = GetTextTexture(ttf_font24_, face_info->photo_name, sdl_color);
//            }
//
//            SDL_Rect clip_rect;
//            clip_rect.x = 0;
//            clip_rect.y = 0;
//            SDL_QueryTexture(face_info->name_texture, nullptr, nullptr, &clip_rect.w, &clip_rect.h);
//
//            if (clip_rect.w < 152)
//            {
//                dst_rect.x = 340 + (152 - clip_rect.w) / 2;
//                dst_rect.y = 56;
//                dst_rect.w = clip_rect.w;
//                dst_rect.h = clip_rect.h;
//            }
//            else
//            {
//                dst_rect.x = 340;
//                dst_rect.y = 56;
//                dst_rect.w = 152;
//                dst_rect.h = clip_rect.h;
//
//                clip_rect.w = 152;
//            }
//
//            RenderTexture(face_info->name_texture, dst_rect, &clip_rect);
//
//            // department
//            if (face_info->department_texture == nullptr)
//            {
//                SDL_Color sdl_color = {0, 255, 255, 255};
//
//                face_info->department_texture = GetTextTexture(ttf_font16_, face_info->photo_department, sdl_color);
//            }
//
//            clip_rect.x = 0;
//            clip_rect.y = 0;
//            SDL_QueryTexture(face_info->department_texture, nullptr, nullptr, &clip_rect.w, &clip_rect.h);
//
//            dst_rect.x = 328;
//            dst_rect.y = 119;
//
//            if (clip_rect.w < 152)
//            {
//                dst_rect.w = clip_rect.w;
//                dst_rect.h = clip_rect.h;
//            }
//            else
//            {
//                dst_rect.w = 152;
//                dst_rect.h = clip_rect.h;
//
//                clip_rect.w = 152;
//            }
//
//            RenderTexture(face_info->department_texture, dst_rect, &clip_rect);
//
//            // id
//            if (face_info->id_texture == nullptr)
//            {
//                SDL_Color sdl_color = {0, 180, 219, 255};
//
//                face_info->id_texture = GetTextTexture(ttf_font16_, face_info->photo_id, sdl_color);
//            }
//
//            RenderTexture(face_info->id_texture, 328, 152);
//
//            // score
//            if (face_info->score_texture == nullptr)
//            {
//                string text_info = to_string(face_info->score) + "%";
//
//                SDL_Color sdl_color = {0, 245, 247, 255};
//                face_info->score_texture = GetTextTexture(ttf_font16_, text_info, sdl_color);
//            }
//
//            RenderTexture(face_info->score_texture, 510, 145);
//
//            SDL_SetRenderTarget(sdl_renderer_, nullptr);
//        }
//
//        SDL_Rect dst_rect;
//        dst_rect.x = 37 + column_width_ * (i / row_number_);
//        dst_rect.y = 96 + row_height_ * (i % row_number_);
//        dst_rect.w = 600;
//        dst_rect.h = 231;
//
//        RenderTexture(face_info->unit_texture, dst_rect);

        // unit background
        SDL_Rect dst_rect;
        dst_rect.x = 37 + column_width_ * (i / row_number_);
        dst_rect.y = 96 + row_height_ * (i % row_number_);
        dst_rect.w = 600;
        dst_rect.h = 231;

        if (face_info->score < face_info->pass_similarity)
        {
            RenderTexture(unit_confirm_texture_, dst_rect.x, dst_rect.y);
        }
        else
        {
            RenderTexture(unit_ok_texture_, dst_rect.x, dst_rect.y);
        }

        // photo
        if (face_info->photo_texture == nullptr)
        {
            SDL_RWops *rwops_src = SDL_RWFromMem(face_info->photo_jpg, face_info->photo_size);
            if (rwops_src != nullptr)
            {
                face_info->photo_texture = IMG_LoadTexture_RW(sdl_renderer_, rwops_src, 1);
            }
        }

        dst_rect.x = 82 + column_width_ * (i / row_number_);
        dst_rect.y = 138 + row_height_ * (i % row_number_);
        dst_rect.w = 106;
        dst_rect.h = 149;

        RenderTexture(face_info->photo_texture, dst_rect);

        // snapshot
        if (face_info->snapshot_size > 0)
        {
            if (face_info->snapshot_texture == nullptr)
            {
                SDL_RWops *rwops_src = SDL_RWFromMem(face_info->snapshot_jpg, face_info->snapshot_size);
                if (rwops_src != nullptr)
                {
                    face_info->snapshot_texture = IMG_LoadTexture_RW(sdl_renderer_, rwops_src, 1);
                }
            }

            dst_rect.x = 208 + column_width_ * (i / row_number_);
            dst_rect.y = 138 + row_height_ * (i % row_number_);
            dst_rect.w = 106;
            dst_rect.h = 149;
            RenderTexture(face_info->snapshot_texture, dst_rect);
        }

        // name
        if (face_info->name_texture == nullptr)
        {
            SDL_Color sdl_color = {208, 248, 255, 255};

            face_info->name_texture = GetTextTexture(ttf_font24_, face_info->photo_name, sdl_color);
        }

        SDL_Rect clip_rect;
        clip_rect.x = 0;
        clip_rect.y = 0;
        SDL_QueryTexture(face_info->name_texture, nullptr, nullptr, &clip_rect.w, &clip_rect.h);

        if (clip_rect.w < 152)
        {
            dst_rect.x = 377 + (152 - clip_rect.w) / 2 + column_width_ * (i / row_number_);
            dst_rect.y = 152 + row_height_ * (i % row_number_);
            dst_rect.w = clip_rect.w;
            dst_rect.h = clip_rect.h;
        }
        else
        {
            dst_rect.x = 377 + column_width_ * (i / row_number_);
            dst_rect.y = 152 + row_height_ * (i % row_number_);
            dst_rect.w = 152;
            dst_rect.h = clip_rect.h;

            clip_rect.w = 152;
        }

        RenderTexture(face_info->name_texture, dst_rect, &clip_rect);

        // department
        if (face_info->department_texture == nullptr)
        {
            SDL_Color sdl_color = {0, 255, 255, 255};

            face_info->department_texture = GetTextTexture(ttf_font16_, face_info->photo_department, sdl_color);
        }

        clip_rect.x = 0;
        clip_rect.y = 0;
        SDL_QueryTexture(face_info->department_texture, nullptr, nullptr, &clip_rect.w, &clip_rect.h);

        dst_rect.x = 365 + column_width_ * (i / row_number_);
        dst_rect.y = 215 + row_height_ * (i % row_number_);

        if (clip_rect.w < 152)
        {
            dst_rect.w = clip_rect.w;
            dst_rect.h = clip_rect.h;
        }
        else
        {
            dst_rect.w = 152;
            dst_rect.h = clip_rect.h;

            clip_rect.w = 152;
        }

        RenderTexture(face_info->department_texture, dst_rect, &clip_rect);

        // id
        if (face_info->id_texture == nullptr)
        {
            SDL_Color sdl_color = {0, 180, 219, 255};

            face_info->id_texture = GetTextTexture(ttf_font16_, face_info->photo_id, sdl_color);
        }

        dst_rect.x = 365 + column_width_ * (i / row_number_);
        dst_rect.y = 248 + row_height_ * (i % row_number_);

        RenderTexture(face_info->id_texture, dst_rect.x, dst_rect.y);

        // score
        if (face_info->score_texture == nullptr)
        {
            string text_info = to_string(face_info->score) + "%";

            SDL_Color sdl_color = {0, 245, 247, 255};
            face_info->score_texture = GetTextTexture(ttf_font16_, text_info, sdl_color);
        }

        dst_rect.x = 547 + column_width_ * (i / row_number_);
        dst_rect.y = 241 + row_height_ * (i % row_number_);

        RenderTexture(face_info->score_texture, dst_rect.x, dst_rect.y);
    }
    face_info_list_.Unlock();
}

void FaceViewWindow::RenderTexture(SDL_Texture *tex, SDL_Rect dst, SDL_Rect *clip)
{
    SDL_RenderCopy(sdl_renderer_, tex, clip, &dst);
}

void FaceViewWindow::RenderTexture(SDL_Texture *tex, int x, int y, SDL_Rect *clip)
{
    SDL_Rect dst;
    dst.x = x;
    dst.y = y;
    if (clip != nullptr)
    {
        dst.w = clip->w;
        dst.h = clip->h;
    }
    else
    {
        SDL_QueryTexture(tex, nullptr, nullptr, &dst.w, &dst.h);
    }

    RenderTexture(tex, dst, clip);
}

SDL_Texture *FaceViewWindow::GetTextTexture(TTF_Font *font, const string &message, SDL_Color color, Uint32 wrap_length)
{
    string log_msg;

    if (message.length() == 0 || font == nullptr)
    {
        return nullptr;
    }

    SDL_Surface *surf = nullptr;

    if (wrap_length == 0)
        surf = TTF_RenderUTF8_Blended(font, message.c_str(), color);
    else
        surf = TTF_RenderUTF8_Blended_Wrapped(font, message.c_str(), color, wrap_length);

    if (surf == nullptr)
    {
        log_msg = "TTF_RenderUTF8_Blended error: ";
        log_msg += SDL_GetError();
        WARNING_LOG(log_msg)

        return nullptr;
    }

    SDL_Texture *texture = SDL_CreateTextureFromSurface(sdl_renderer_, surf);
    if (texture == nullptr)
    {
        log_msg = "SDL_CreateTextureFromSurface error: ";
        log_msg += SDL_GetError();
        WARNING_LOG(log_msg)
    }

    SDL_FreeSurface(surf);

    return texture;
}

TTF_Font *FaceViewWindow::GetFont(const string &font_file, int font_size)
{
    string log_msg;

    TTF_Font *font = TTF_OpenFont(font_file.c_str(), font_size);
    if (font == nullptr)
    {
        log_msg = "TTF_OpenFont error: ";
        log_msg += SDL_GetError();
        WARNING_LOG(log_msg)
    }

    return font;
}

SDL_Texture *FaceViewWindow::LoadTexture(const string &image_file)
{
    string log_msg;

    SDL_Texture *texture = IMG_LoadTexture(sdl_renderer_, image_file.c_str());
    if (texture == nullptr)
    {
        log_msg = "IMG_LoadTexture error: ";
        log_msg += SDL_GetError();
        WARNING_LOG(log_msg)
    }

    return texture;
}

Mix_Music *FaceViewWindow::LoadMusic(const string &music_file)
{
    string log_msg;

    Mix_Music *mix_music = Mix_LoadMUS(music_file.c_str());
    if (mix_music == nullptr)
    {
        log_msg = "Mix_LoadMUS error: ";
        log_msg += SDL_GetError();
        WARNING_LOG(log_msg)
    }

    return mix_music;
}

void FaceViewWindow::AddFaceInfo(PFaceInfo face_info)
{
    bool find_flag = false;

    if (face_info != nullptr && face_info->person_id != 0)
    {
        face_info_list_.Lock();
        for (unsigned int i = 0; i < face_info_list_.GetSize(); i++)
        {
            PFaceInfo p_find = face_info_list_.GetAt(i);
            if (p_find->person_id == face_info->person_id)
            {
                find_flag = true;

                face_info_list_.InsertAt(i, face_info);
                face_info_list_.RemoveAt(i + 1);

                delete p_find;

                break;
            }
        }

        face_info_list_.Unlock();
    }

    if (!find_flag)
    {
        face_info_list_.Add(face_info);
    }

    if (face_info->score < face_info->pass_similarity)
    {
        Mix_PlayMusic(music_confirm_, 1);
    }
    else
    {
        Mix_PlayMusic(music_ok_, 1);
    }
}

void FaceViewWindow::SetRenderBlack()
{
    SDL_SetRenderDrawColor(sdl_renderer_, 0, 0, 0, 255);
}
