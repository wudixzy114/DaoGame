#include <SDL3/SDL.h>
#include <ft2build.h>
#include FT_FREETYPE_H
#include <hb.h>
#define MINIAUDIO_IMPLEMENTATION
#include <miniaudio.h>
#include <nlohmann/json.hpp>

#include <array>
#include <iostream>
#include <stdexcept>
#include <string>

namespace {

class SdlSmokeApp {
public:
    SdlSmokeApp()
    {
        if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_EVENTS)) {
            throw std::runtime_error(std::string("SDL_Init failed: ") + SDL_GetError());
        }

        window_ = SDL_CreateWindow("DaoGame Dependency Smoke", 640, 360, SDL_WINDOW_HIDDEN);
        if (window_ == nullptr) {
            throw std::runtime_error(std::string("SDL_CreateWindow failed: ") + SDL_GetError());
        }

        renderer_ = SDL_CreateRenderer(window_, nullptr);
        if (renderer_ == nullptr) {
            throw std::runtime_error(std::string("SDL_CreateRenderer failed: ") + SDL_GetError());
        }
    }

    ~SdlSmokeApp()
    {
        if (renderer_ != nullptr) {
            SDL_DestroyRenderer(renderer_);
        }
        if (window_ != nullptr) {
            SDL_DestroyWindow(window_);
        }
        SDL_Quit();
    }

    SdlSmokeApp(const SdlSmokeApp&) = delete;
    SdlSmokeApp& operator=(const SdlSmokeApp&) = delete;

    void render_frames(int frame_count)
    {
        for (int frame = 0; frame < frame_count; ++frame) {
            SDL_Event event{};
            while (SDL_PollEvent(&event)) {
            }

            SDL_SetRenderDrawColor(renderer_, 14, 16, 20, 255);
            SDL_RenderClear(renderer_);

            SDL_FRect panel{48.0F, 48.0F, 220.0F, 96.0F};
            SDL_SetRenderDrawColor(renderer_, 36, 43, 54, 255);
            SDL_RenderFillRect(renderer_, &panel);

            SDL_FRect die{72.0F, 72.0F, 48.0F, 48.0F};
            SDL_SetRenderDrawColor(renderer_, 211, 180, 104, 255);
            SDL_RenderFillRect(renderer_, &die);

            SDL_RenderPresent(renderer_);
        }
    }

private:
    SDL_Window* window_{nullptr};
    SDL_Renderer* renderer_{nullptr};
};

void verify_third_party_headers()
{
    FT_Library freetype{};
    if (FT_Init_FreeType(&freetype) != 0) {
        throw std::runtime_error("FT_Init_FreeType failed");
    }
    FT_Done_FreeType(freetype);

    hb_buffer_t* buffer = hb_buffer_create();
    hb_buffer_add_utf8(buffer, "DaoGame", -1, 0, -1);
    hb_buffer_guess_segment_properties(buffer);
    hb_buffer_destroy(buffer);

    ma_engine_config config = ma_engine_config_init();
    (void)config;

    const nlohmann::json manifest = {
        {"target", "DaoGameDependencySmoke"},
        {"libraries", std::array{"SDL3", "FreeType", "HarfBuzz", "miniaudio", "nlohmann-json"}}
    };
    std::cout << manifest.dump() << '\n';
}

} // namespace

int main()
{
    try {
        verify_third_party_headers();
        SdlSmokeApp app;
        app.render_frames(3);
    } catch (const std::exception& error) {
        std::cerr << error.what() << '\n';
        return 1;
    }

    return 0;
}
