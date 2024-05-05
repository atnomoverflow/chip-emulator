#pragma once
#include <iostream>
#include <fstream>
#include <SDL2/SDL.h>
#include "./CPU/CPU.h"

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

public:
    bool init();
    void start();
    Emulator(CPU &cpu, Memory &memory, Config &config, const char rom_name[]);
    ~Emulator();
};

Emulator::Emulator(CPU &cpu, Memory &memory, Config &config, const char rom_name[])
    : m_window(nullptr), m_renderer(nullptr), m_configuration(config), m_cpu(cpu), m_memory(memory)
{
    load(rom_name);
}

Emulator::~Emulator()
{
    // Release resources
    if (m_renderer)
    {
        SDL_DestroyRenderer(m_renderer);
    }
    if (m_window)
    {
        SDL_DestroyWindow(m_window);
    }
    SDL_Quit();
}
