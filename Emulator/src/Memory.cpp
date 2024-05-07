#include "Memory.hpp"
namespace Emulator
{
    Memory::Memory(Config &config)
        : m_config(config), m_RAM{}, m_STACK{}, DISPLAY(nullptr)
    {
        display_size = m_config.HEIGHT * m_config.WIDTH;
        DISPLAY = new bool[display_size];
        const uint8_t font[] = {
            0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
            0x20, 0x60, 0x20, 0x20, 0x70, // 1
            0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
            0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
            0x90, 0x90, 0xF0, 0x10, 0x10, // 4
            0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
            0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
            0xF0, 0x10, 0x20, 0x40, 0x40, // 7
            0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
            0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
            0xF0, 0x90, 0xF0, 0x90, 0x90, // A
            0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
            0xF0, 0x80, 0x80, 0x80, 0xF0, // C
            0xE0, 0x90, 0x90, 0x90, 0xE0, // D
            0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
            0xF0, 0x80, 0xF0, 0x80, 0x80  // F
        };
        std::memcpy(&m_RAM[0], font, sizeof(font));
    }

    Memory::~Memory()
    {
        delete DISPLAY;
    }
    void Memory::load_rom(std::ifstream &file, const size_t file_size)
    {
        char *rom = new char[file_size];
        file.seekg(0, std::ios::beg);
        file.read(rom, file_size);
        std::memcpy(m_RAM + ENTRY_POINT, rom, file_size);
        delete[] rom;
    }
    size_t Memory::get_memory_size()
    {
        return sizeof(m_RAM);
    }

    uint16_t Memory::get_instruction(const uint16_t pc)
    {
        return (m_RAM[pc] << 8) | (m_RAM[pc + 1]);
    }
    uint8_t Memory::get_data(const size_t I)
    {
        return m_RAM[I];
    }
    void Memory::set_data(const size_t I, const uint8_t data)
    {
        m_RAM[I] = data;
    }
    uint16_t *Memory::get_stack_pointer()
    {
        return &m_STACK[0];
    }
} // namespace Emulator