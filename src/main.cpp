#include "Emulator/Emulator.cpp"
#include <stdio.h>
int main(int argc, char **argv)
{

    if (argc < 2)
    {
        std::cerr << "Usage:  chip8 <ROM_PATH> ";
        return 1;
    }
    char *rom_name = argv[1];
    Config conf;
    Memory memory(conf);
    CPU cpu(memory, conf);
    Emulator emulator(cpu, memory, conf, rom_name);

    if (!emulator.init())
    {
        return 1;
    }
    emulator.start();
    return 0;
}
