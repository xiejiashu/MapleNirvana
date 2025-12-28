#pragma once

#include <string>
#include "NumTypes.hpp"

namespace wz {

    using wzstring = std::u16string;

    struct WzNull {
    };

    struct WzSubProp {
    };

    struct WzConvex {
    };

    struct WzUOL {
        [[maybe_unused]]
        wzstring uol;
    };

    struct WzCanvas {
        i32 width;
        i32 height;
        i32 format;
        i32 format2;
        bool is_encrypted;
        i32 size;
        i32 uncompressed_size;
        size_t offset;

        WzCanvas()
                : width(0), height(0),
                  format(0), format2(0),
                  is_encrypted(false), size(0), uncompressed_size(0),
                  offset(0) {}
    };

    struct WzSound {
        i32 length;
        i32 frequency;
        i32 size;
        size_t offset;

        WzSound() : length(0), frequency(0), size(0), offset(0) {}
    };

    struct WzVec2D {

        i32 x;
        i32 y;

        WzVec2D() : x(0), y(0) {}
        WzVec2D(int new_x, int new_y) : x(new_x), y(new_y) {}
    };
}