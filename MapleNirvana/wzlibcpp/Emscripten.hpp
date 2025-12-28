#pragma once

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include <emscripten/fetch.h>
#include <string>
#include <iostream>

namespace wz
{
    class Emscripten
    {
    public:
        static inline unsigned char *file_data = nullptr;
        static inline unsigned int file_size = 0;
        static inline volatile bool fetch_done = false; // 用于控制是否完成请求

    public:
        static void fetch_file_success(emscripten_fetch_t *fetch);

        // 请求失败时的回调函数
        static void fetch_file_failure(emscripten_fetch_t *fetch);

        static void load_file(std::string &url);
        static void load_file(std::u16string &url);
    };
}
#endif