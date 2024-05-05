#include "./Emulator.h"

bool Emulator::init()
{
    // Initialize SDL logging
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER) != 0)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL initialization failed: %s", SDL_GetError());
        return false;
    }

    m_window = SDL_CreateWindow(m_configuration.TITLE,
                                SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                m_configuration.WIDTH * m_configuration.SCALE,
                                m_configuration.HEIGHT * m_configuration.SCALE,
                                SDL_WINDOW_SHOWN);
    if (!m_window)
    {
        // Window creation failed
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL window creation failed: %s", SDL_GetError());
        SDL_Quit();
        return false;
    }

    m_renderer = SDL_CreateRenderer(m_window, -1, SDL_RENDERER_ACCELERATED);
    if (!m_renderer)
    {
        // Renderer creation failed
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL renderer creation failed: %s", SDL_GetError());
        SDL_DestroyWindow(m_window);
        SDL_Quit();
        return false;
    }
    if (SDL_RenderClear(m_renderer))
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL renderer creation failed: %s", SDL_GetError());
        SDL_DestroyRenderer(m_renderer);
        SDL_DestroyWindow(m_window);
        return false;
    }

    return true;
}

bool Emulator::clear_screan()
{
    const uint8_t r = (m_configuration.BG_COLOR >> 24) & 0xFF;
    const uint8_t g = (m_configuration.BG_COLOR >> 16) & 0xFF;
    const uint8_t b = (m_configuration.BG_COLOR >> 8) & 0xFF;
    const uint8_t a = m_configuration.BG_COLOR & 0xFF;

    if (SDL_SetRenderDrawColor(m_renderer, r, g, b, a))
    {
        return false;
    }

    if (SDL_RenderClear(m_renderer) != 0)
    {
        return false;
    }
    return true;
}
void Emulator::handle_event()
{
    while (SDL_PollEvent(&m_event))
    {
        switch (m_event.type)
        {
        case SDL_QUIT:
            m_state = STOPPED;
            break;
        case SDL_KEYUP:
            switch (m_event.key.keysym.sym)
            {
            case SDLK_ESCAPE:
                m_state = STOPPED;
                break;
            case SDLK_SPACE:
            {
                if (m_state == RUNNING)
                {
                    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "PAUSING");
                    m_state = PAUSED;
                }
                else if (m_state == PAUSED)
                {
                    m_state = RUNNING;
                    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "RUNNING");
                }
                break;
            }
            default:
                break;
            }
        default:
            break;
        }
    }
}
void Emulator::update_screan()
{

    const uint8_t bg_r = (m_configuration.BG_COLOR >> 24) & 0xFF;
    const uint8_t bg_g = (m_configuration.BG_COLOR >> 16) & 0xFF;
    const uint8_t bg_b = (m_configuration.BG_COLOR >> 8) & 0xFF;
    const uint8_t bg_a = m_configuration.BG_COLOR & 0xFF;

    const uint8_t fg_r = (m_configuration.FG_COLOR >> 24) & 0xFF;
    const uint8_t fg_g = (m_configuration.FG_COLOR >> 16) & 0xFF;
    const uint8_t fg_b = (m_configuration.FG_COLOR >> 8) & 0xFF;
    const uint8_t fg_a = m_configuration.FG_COLOR & 0xFF;

    SDL_Rect pixel = {.x = 0, .y = 0, .w = (int)m_configuration.SCALE, .h = (int)m_configuration.SCALE};

    for (uint32_t i = 0; i < m_memory.display_size; i++)
    {
        pixel.x = (i % m_configuration.WIDTH) * m_configuration.SCALE;

        pixel.y = (i / m_configuration.WIDTH) * m_configuration.SCALE;
        if ((i % m_configuration.WIDTH) == 0)
            pixel.y += m_configuration.SCALE;
        if (m_memory.DISPLAY[i])
        {
            // draw sprit pixel
            SDL_SetRenderDrawColor(m_renderer, fg_r, fg_g, fg_b, fg_a);
            SDL_RenderFillRect(m_renderer, &pixel);
        }
        else
        {
            // draw background pixel
            SDL_SetRenderDrawColor(m_renderer, bg_r, bg_g, bg_b, bg_a);
            SDL_RenderFillRect(m_renderer, &pixel);
        }
    }
    SDL_RenderPresent(m_renderer);
}
void Emulator::start()
{
    clear_screan();
    m_state = RUNNING;
    while (m_state != STOPPED)
    {
        handle_event();
        if (m_state == PAUSED)
            continue;
        m_cpu.fetch();
        OP_CODE op = m_cpu.decode();
        m_cpu.execute(op);
        SDL_Delay(12);
        update_screan();
    }
}
bool Emulator::load(const char rom_name[])
{
    std::streampos begin, end;
    std::ifstream rom(rom_name, std::ios::binary);

    if (!rom)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to open file: %s", rom_name);
        return false;
    }
    begin = rom.tellg();
    rom.seekg(0, std::ios::end);
    end = rom.tellg();
    size_t rom_size = end - begin;
    size_t max_size = m_memory.get_memory_size();
    if (rom_size > max_size)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "File %s size (%zu) too big. Max size: %zu", rom_name, rom_size, max_size);
        rom.close();
        return false;
    }
    rom.clear();
    m_memory.load_rom(rom, rom_size);
    rom.close();
    return true;
}