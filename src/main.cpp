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

class SdlApp {
public:
    SdlApp()
    {
        if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_EVENTS)) {
            throw std::runtime_error(std::string("SDL_Init failed: ") + SDL_GetError());
        }

        window_ = SDL_CreateWindow("DaoGame Runtime", 1280, 720, SDL_WINDOW_RESIZABLE);
        if (window_ == nullptr) {
            throw std::runtime_error(std::string("SDL_CreateWindow failed: ") + SDL_GetError());
        }

        renderer_ = SDL_CreateRenderer(window_, nullptr);
        if (renderer_ == nullptr) {
            throw std::runtime_error(std::string("SDL_CreateRenderer failed: ") + SDL_GetError());
        }
    }

    ~SdlApp()
    {
        if (renderer_ != nullptr) {
            SDL_DestroyRenderer(renderer_);
        }
        if (window_ != nullptr) {
            SDL_DestroyWindow(window_);
        }
        SDL_Quit();
    }

    SdlApp(const SdlApp&) = delete;
    SdlApp& operator=(const SdlApp&) = delete;

    void run(int max_frames = -1)
    {
        bool running = true;
        int frame_count = 0;
        while (running) {
            SDL_Event event{};
            while (SDL_PollEvent(&event)) {
                if (event.type == SDL_EVENT_QUIT) {
                    running = false;
                }
            }

            SDL_SetRenderDrawColor(renderer_, 14, 16, 20, 255);
            SDL_RenderClear(renderer_);

            SDL_FRect panel{48.0F, 48.0F, 420.0F, 160.0F};
            SDL_SetRenderDrawColor(renderer_, 36, 43, 54, 255);
            SDL_RenderFillRect(renderer_, &panel);

            SDL_FRect die{72.0F, 88.0F, 64.0F, 64.0F};
            SDL_SetRenderDrawColor(renderer_, 211, 180, 104, 255);
            SDL_RenderFillRect(renderer_, &die);

            SDL_RenderPresent(renderer_);
            SDL_Delay(16);

            ++frame_count;
            if (max_frames >= 0 && frame_count >= max_frames) {
                running = false;
            }
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
        {"runtime", "DaoGame"},
        {"libraries", std::array{"SDL3", "FreeType", "HarfBuzz", "miniaudio", "nlohmann-json"}}
    };
    std::cout << manifest.dump() << '\n';
}

} // namespace

int main(int argc, char** argv)
{
    try {
        const bool smoke_test = argc > 1 && std::string(argv[1]) == "--smoke-test";
        verify_third_party_headers();
        SdlApp app;
        app.run(smoke_test ? 3 : -1);
    } catch (const std::exception& error) {
        std::cerr << error.what() << '\n';
        return 1;
    }

    return 0;
}
