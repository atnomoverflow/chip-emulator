#pragma once
#include <iostream>
#include <fstream>
#include <SDL2/SDL.h>
#include "CPU.hpp"
namespace Emulator
{
    typedef enum
    {
        STOPPED,
        RUNNING,
        PAUSED,
    } EMULATOR_STATE;

    class Emulator
    {
    private:
        SDL_Window *m_window;
        SDL_Renderer *m_renderer;
        SDL_Event m_event;
        Config &m_configuration;
        Memory &m_memory;
        CPU &m_cpu;
        EMULATOR_STATE m_state = STOPPED;
        char *m_rom;

        bool clear_screan();
        void update_screan();
        void handle_event();
        bool load(const char rom_name[]);
        void dumpToFile(bool arr[], int size, const std::string &filename);
        void update_timer();
    public:
        bool init();
        void start();
        Emulator(CPU &cpu, Memory &memory, Config &config, const char rom_name[]);
        ~Emulator();
    };


} // namespace Emulator
