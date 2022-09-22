#ifndef NTH_UTIL_COLOR_HPP
#define NTH_UTIL_COLOR_HPP

#include <cstdint>

namespace Nth {
    struct Color {
        Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);

        uint8_t r;
        uint8_t g;
        uint8_t b;
        uint8_t a;
    };
}

#endif