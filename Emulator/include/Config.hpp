#pragma once
#include <stdint.h> 
namespace Emulator
{
    struct Config
    {
        static constexpr int TITLE_SIZE = 7;
        const char TITLE[TITLE_SIZE] = "chip-8";
        uint16_t WIDTH = 64;
        uint16_t HEIGHT = 32;
        uint32_t FG_COLOR = 0xFFFFFFFF;
        uint32_t BG_COLOR = 0x000000FF;
        uint32_t SCALE = 20;
        uint32_t CLOCK;
    };
}
