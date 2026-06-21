cmake_minimum_required(VERSION 3.28)

project(DaoGame VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(SDL3 CONFIG REQUIRED)
find_package(Freetype REQUIRED)
find_package(harfbuzz CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(Stb REQUIRED)
find_path(MINIAUDIO_INCLUDE_DIRS "miniaudio.h" REQUIRED)

add_executable(DaoGame
    src/main.cpp
)

target_compile_features(DaoGame PRIVATE cxx_std_20)

target_include_directories(DaoGame
    PRIVATE
        ${Stb_INCLUDE_DIR}
        ${MINIAUDIO_INCLUDE_DIRS}
)

target_link_libraries(DaoGame
    PRIVATE
        SDL3::SDL3
        Freetype::Freetype
        harfbuzz::harfbuzz
        nlohmann_json::nlohmann_json
)

if(MSVC)
    target_compile_options(DaoGame PRIVATE /W4 /permissive-)
else()
    target_compile_options(DaoGame PRIVATE -Wall -Wextra -Wpedantic)
endif()
