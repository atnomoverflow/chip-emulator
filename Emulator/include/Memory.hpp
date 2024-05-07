
#pragma once
#include <cstring>
#include <iostream>
#include "Config.hpp"
#include <fstream>
namespace Emulator
{

    class Memory
    {
    private:
        /// @brief Memory Layout for Chip-8 4096 of 8 bits word
        uint8_t m_RAM[4096];

        uint16_t m_STACK[12];
        Config &m_config;

    public:
        bool *DISPLAY;
        const uint16_t ENTRY_POINT = 0x200;
        size_t display_size; // Store the size of the DISPLAY array
        Memory(Config &config);
        ~Memory();
        size_t get_memory_size();
        uint16_t *get_stack_pointer();
        uint16_t get_instruction(uint16_t pc);
        uint8_t get_data(size_t I);
        void set_data(const size_t I, const uint8_t data);

        void load_rom(std::ifstream &file, size_t);
    };
} // namespace Emulator
