#include <SDL2/SDL.h>
#include <format>
#include "../Config.h"
#include "../Memory/Memory.h"
#include <stdio.h>
#include <string>
#include <iostream>

#define LOG(message) std::cout << "[DEBUG] " << message << std::endl

typedef struct
{
    uint16_t op;
    uint16_t NNN = op << 12;
    uint8_t NN = (op << 8) & 0xFF;
    uint8_t N = (op << 8) & 0xFF;
    uint8_t X = (op << 8) & 0xFF;
    uint8_t Y = (op << 8) & 0xFF;

} Instruction;
typedef enum
{
    CLEAR_SCREAN,
    EXECUTE_SUB_NNN,
    RETURN_FROM_SUB,
    CALL_SUB_NNN,
    SKIP_IF_VX_EQ_NN,
    SKIP_IF_VX_NEQ_NN,
    SKIP_IF_VX_EQ_VY,
    JUMP_NNN,
    JUMP_TO_V0_NNN,
    SET_I_TO_NNN,
    SET_VX_TO_NN,
    ADD_NN_TO_VX,
    SET_VX_TO_VY,
    VX_OR_VY,
    VX_AND_VY,
    VX_XOR_VY,
    ADD_VX_TO_VY,
    SUB_VX_TO_VY,
    DRAW,
    ADD_VX_TO_I,
    SET_VX_TO_KEYPAD,
    UNKNOWN
} OP_CODE;
class CPU
{
private:
    /* data */
    uint16_t m_PC = 0x200;
    uint16_t *m_SP;
    uint8_t m_V[16];
    uint16_t m_I;
    uint8_t m_delay_timer;
    uint8_t m_sound_timer;
    Instruction m_current_instruction;
    Memory &m_memory;
    Config m_config;
    bool m_KEY_PAD[16];

    void clear_screen();
    void return_from_sub();
    void jump_nnn();
    void set_i_to_nnn();
    void set_vx_to_nn();
    void add_vx_to_I();
    void set_vx_to_keypad();
    void call_sub_at_nnn();
    void skip_if_vx_eq_nn();
    void skip_if_vx_neq_nn();
    void add_nn_to_vx();
    void skip_if_vx_eq_vy();
    void set_vx_to_vy();
    void vx_or_vy();
    void vx_and_vy();
    void vx_xor_vy();
    void add_vx_to_vy();
    void sub_vx_to_vy();
    void jump_to_v0_nnn();
    void display();

public:
    OP_CODE decode();
    void execute(OP_CODE op);
    void fetch();

    CPU(Memory &memory, Config &config);
    ~CPU();
};

CPU::CPU(Memory &memory, Config &config)
    : m_memory(memory), m_config(config)
{
    m_SP = m_memory.get_stack_pointer();
}

CPU::~CPU()
{
}

// should get next instruction and increase PC
// is it okay to have side effect i hate side effect
void CPU::fetch()
{
    uint16_t op = m_memory.get_instruction(m_PC);
    m_current_instruction.op = op;
    m_current_instruction.NNN = op & 0x0FFF;
    m_current_instruction.NN = op & 0x0FF;
    m_current_instruction.N = op & 0x0F;
    m_current_instruction.X = (op >> 8) & 0x0F;
    m_current_instruction.Y = (op >> 4) & 0x0F;

    // inc the pc by 2 bytes
    m_PC += 2;
}

// should return enum of the instruction
// the args are in the instruction anyway right ??
OP_CODE CPU::decode()
{
    uint8_t opcode = (m_current_instruction.op >> 12) & 0x0F;
    switch (opcode)
    {
    case 0x0:
        switch (m_current_instruction.NNN)
        {
        case 0x0E0:
            return CLEAR_SCREAN;
        case 0x0EE:
            return RETURN_FROM_SUB;
        default:
            return EXECUTE_SUB_NNN;
        }
    case 0x01:
        return JUMP_NNN;
    case 0x02:
        return CALL_SUB_NNN;
    case 0x03:
        return SKIP_IF_VX_EQ_NN;
    case 0x04:
        return SKIP_IF_VX_NEQ_NN;
    case 0x05:
        return SKIP_IF_VX_EQ_VY;
    case 0x06:
        return SET_VX_TO_NN;
    case 0x07:
        return ADD_NN_TO_VX;
    case 0x08:
        switch (m_current_instruction.N)
        {
        case 0x0:
            return SET_VX_TO_VY;
        case 0x1:
            return VX_OR_VY;
        case 0x2:
            return VX_AND_VY;
        case 0x3:
            return VX_XOR_VY;
        case 0x4:
            return ADD_VX_TO_VY;
        case 0x5:
            return SUB_VX_TO_VY;
            // case 0x6:
            //     return SUB_VX_TO_VY;

        default:
            break;
        }
    case 0x0A:
        return SET_I_TO_NNN;
    case 0x0B:
        return JUMP_TO_V0_NNN;
    case 0x0D:
        return DRAW;
    case 0x0F:
        switch (m_current_instruction.NN)
        {
        case 0x1E:
            return ADD_VX_TO_I;
        case 0x0A:
            SET_VX_TO_KEYPAD;
        default:
            break;
        }
    default:
        break;
    }
    return UNKNOWN;
}

// should be an ugly switch containing the instruction sets
// it's gonna be uggly but matching enums should be less ugly i think
void CPU::execute(OP_CODE op)
{
    switch (op)
    {
    case CLEAR_SCREAN:
    {

        LOG(std::format("op: 0x{:04X} ,address: 0x{:04X} , Desc: Clear screen \n", m_current_instruction.op, m_PC));
        clear_screen();
        break;
    }
    case CALL_SUB_NNN:
    {
        LOG(std::format("op: 0x{:04X} ,address: 0x{:04X} , Desc: Call Sub routine NNN: 0x{:04X} \n", m_current_instruction.op, m_PC, m_current_instruction.NNN));
        call_sub_at_nnn();
        break;
    }
    case EXECUTE_SUB_NNN:
    {
        LOG(std::format("op: 0x{:04X},address: 0x{:04X}, Desc: execute sub rotine at NNN: 0x{:04} (we are skipping this !!!!) \n", m_current_instruction.op, m_PC, m_current_instruction.NNN));
        break;
    }
    case RETURN_FROM_SUB:
    {
        LOG(std::format("op: 0x{:04X},address: 0x{:04X} , Desc: Return from sub rotine to address 0x{:04X}\n", m_current_instruction.op, m_PC, *(m_SP - 1)));
        return_from_sub();
        break;
    }
    case JUMP_NNN:
    {
        LOG(std::format("op: 0x{:04X},address: 0x{:04X}, Desc: Jump to NNN: 0x{:04X}\n", m_current_instruction.op, m_PC, m_current_instruction.NNN));
        jump_nnn();
        break;
    }
    case SET_I_TO_NNN:
    {
        LOG(std::format("op: 0x{:04X},address: 0x{:04X}, Desc: SET I TO NNN: 0x{:04X} \n", m_current_instruction.op, m_PC, m_current_instruction.NNN));
        set_i_to_nnn();
        break;
    }
    case SET_VX_TO_NN:
    {
        LOG(std::format("op: 0x{:04X},address: 0x{:04X}, Desc: SET V{} TO NN: 0x{:04X} \n", m_current_instruction.op, m_PC, m_current_instruction.X, m_current_instruction.NN));
        set_vx_to_nn();
        break;
    }
    case ADD_VX_TO_I:
    {
        LOG(std::format("op: 0x{:04X},address: 0x{:04X}, Desc: ADD V{} TO I: 0x{:04X} \n", m_current_instruction.op, m_PC, m_current_instruction.X, m_I));
        add_vx_to_I();
        break;
    }
    case SET_VX_TO_KEYPAD:
    {
        LOG(std::format("op: 0x{:04X},address: 0x{:04X}, Desc: SET V{} TO KEYPAD ??? \n", m_current_instruction.op, m_PC, m_current_instruction.X));
        set_vx_to_keypad();
        break;
    }
    case DRAW:
    {
        LOG(std::format("op: 0x{:04X},address: 0x{:04X}, Desc: DRAW \n", m_current_instruction.op, m_PC));
        display();
        break;
    }
    case SKIP_IF_VX_EQ_NN:
    {
        LOG(std::format("op: 0x{:04X},address: 0x{:04X}, Desc: Skip if V{}: 0x{:04X} == NN: 0x{:04X} \n", m_current_instruction.op, m_PC, m_current_instruction.X, m_V[m_current_instruction.X], m_current_instruction.NN));
        skip_if_vx_eq_nn();
        break;
    }
    case SKIP_IF_VX_NEQ_NN:
    {
        LOG(std::format("op: 0x{:04X},address: 0x{:04X}, Desc: Skip if V{}: 0x{:04X} != NN: 0x{:04X} \n", m_current_instruction.op, m_PC, m_current_instruction.X, m_V[m_current_instruction.X], m_current_instruction.NN));
        skip_if_vx_neq_nn();
        break;
    }
    case SKIP_IF_VX_EQ_VY:
    {
        LOG(std::format("op: 0x{:04X},address: 0x{:04X}, Desc: Skip if V{}: 0x{:04X} == V{}: 0x{:04X} \n", m_current_instruction.op, m_PC, m_current_instruction.X, m_V[m_current_instruction.X], m_current_instruction.Y, m_V[m_current_instruction.Y]));
        skip_if_vx_eq_vy();
        break;
    }
    case ADD_NN_TO_VX:
    {
        LOG(std::format("op: 0x{:04X},address: 0x{:04X}, Desc: Add NN: 0x{:04X} to V{}: 0x{:04X} \n", m_current_instruction.op, m_PC, m_current_instruction.NN, m_current_instruction.X, m_V[m_current_instruction.X]));
        add_nn_to_vx();
        break;
    }
    case SET_VX_TO_VY:
    {
        LOG(std::format("op: 0x{:04X},address: 0x{:04X}, Desc: Set V{}: 0x{:04X} = V{}: 0x{:04X} \n", m_current_instruction.op, m_PC, m_current_instruction.X, m_V[m_current_instruction.X], m_current_instruction.Y, m_V[m_current_instruction.Y]));
        set_vx_to_vy();
        break;
    }
    case VX_OR_VY:
    {
        LOG(std::format("op: 0x{:04X},address: 0x{:04X}, Desc: Set V{}: 0x{:04X} |= V{}: 0x{:04X} \n", m_current_instruction.op, m_PC, m_current_instruction.X, m_V[m_current_instruction.X], m_current_instruction.Y, m_V[m_current_instruction.Y]));
        vx_or_vy();
        break;
    }
    case VX_AND_VY:
    {
        LOG(std::format("op: 0x{:04X},address: 0x{:04X}, Desc: Set V{}: 0x{:04X} &= V{}: 0x{:04X} \n", m_current_instruction.op, m_PC, m_current_instruction.X, m_V[m_current_instruction.X], m_current_instruction.Y, m_V[m_current_instruction.Y]));
        vx_and_vy();
        break;
    }
    case VX_XOR_VY:
    {
        LOG(std::format("op: 0x{:04X},address: 0x{:04X}, Desc: Set V{}: 0x{:04X} ^= V{}: 0x{:04X} \n", m_current_instruction.op, m_PC, m_current_instruction.X, m_V[m_current_instruction.X], m_current_instruction.Y, m_V[m_current_instruction.Y]));
        vx_xor_vy();
        break;
    }
    case ADD_VX_TO_VY:
    {
        LOG(std::format("op: 0x{:04X},address: 0x{:04X}, Desc: Set V{}: 0x{:04X} += V{}: 0x{:04X} \n", m_current_instruction.op, m_PC, m_current_instruction.X, m_V[m_current_instruction.X], m_current_instruction.Y, m_V[m_current_instruction.Y]));
        add_vx_to_vy();
        break;
    }
    case SUB_VX_TO_VY:
    {
        LOG(std::format("op: 0x{:04X},address: 0x{:04X}, Desc: Set V{}: 0x{:04X} -= V{}: 0x{:04X} \n", m_current_instruction.op, m_PC, m_current_instruction.X, m_V[m_current_instruction.X], m_current_instruction.Y, m_V[m_current_instruction.Y]));
        sub_vx_to_vy();
        break;
    }
    case JUMP_TO_V0_NNN:
    {
        LOG(std::format("op: 0x{:04X},address: 0x{:04X}, Desc: JUMP to V0: 0x{:04X} + NNN: 0x{:04X} \n", m_current_instruction.op, m_PC, m_V[0], m_current_instruction.NNN));
        jump_to_v0_nnn();
        break;
    }
    default:
        LOG(std::format("op: 0x{:04X},address: 0x{:04X}, Desc: UNIMPLIMENTED", m_current_instruction.op, m_PC));
        break;
    }
}
void CPU::clear_screen()
{
    for (size_t i = 0; i < m_memory.display_size; i++)
    {
        m_memory.DISPLAY[i] = false;
    }
}

void CPU::return_from_sub()
{
    m_PC = *--m_SP;
}
void CPU::jump_nnn()
{
    m_PC = m_current_instruction.NNN;
}
void CPU::set_i_to_nnn()
{
    m_I = m_current_instruction.NNN;
}
void CPU::set_vx_to_nn()
{
    m_V[m_current_instruction.X] = m_current_instruction.NN;
}

void CPU::add_vx_to_I()
{
    m_I += m_V[m_current_instruction.X];
}

void CPU::set_vx_to_keypad()
{
}
void CPU::call_sub_at_nnn()
{
    *m_SP++ = m_PC;
    m_PC = m_current_instruction.NNN;
}
void CPU::display()
{
    int x = m_V[m_current_instruction.X] % m_config.WIDTH;
    int y = m_V[m_current_instruction.Y] % m_config.HEIGHT;
    int orig_x = x;
    m_V[0xF] = 0; // VF carry flag

    for (uint8_t i = 0; i < m_current_instruction.N; i++)
    {
        // get sprit data from ram
        const uint8_t sprit = m_memory.get_data((m_I + i));
        x = orig_x;
        for (uint8_t j = 7; j >= 0; j--)
        {
            bool *pixel = &m_memory.DISPLAY[y * m_config.WIDTH + x];
            const bool sprit_pixel=(sprit & (1 << j));
            // for collistion detection i think ???
            if (sprit_pixel && *pixel)
            {
                m_V[0xF] = 1;
            }
            *pixel ^= sprit_pixel;
            if (++x >= m_config.WIDTH)
                break;
        }
        if (++y >= m_config.HEIGHT)
            break;
    }
}

void CPU::skip_if_vx_eq_nn()
{
    if (m_current_instruction.NN == m_V[m_current_instruction.X])
    {
        m_PC += 2;
    }
}

void CPU::skip_if_vx_neq_nn()
{
    if (m_current_instruction.NN != m_V[m_current_instruction.X])
    {
        m_PC += 2;
    }
}

void CPU::add_nn_to_vx()
{
    m_V[m_current_instruction.X] += m_current_instruction.NN;
}

void CPU::skip_if_vx_eq_vy()
{
    if (m_V[m_current_instruction.Y] == m_V[m_current_instruction.X])
    {
        m_PC += 2;
    }
}

void CPU::set_vx_to_vy()
{
    m_V[m_current_instruction.X] = m_V[m_current_instruction.Y];
}
void CPU::vx_or_vy()
{
    m_V[m_current_instruction.X] |= m_V[m_current_instruction.Y];
}
void CPU::vx_and_vy()
{
    m_V[m_current_instruction.X] &= m_V[m_current_instruction.Y];
}
void CPU::vx_xor_vy()
{
    m_V[m_current_instruction.X] ^= m_V[m_current_instruction.Y];
}
void CPU::add_vx_to_vy()
{
    m_V[0xF] = (uint16_t)(m_V[m_current_instruction.Y] + m_V[m_current_instruction.X]) > 255;
    m_V[m_current_instruction.X] += m_V[m_current_instruction.Y];
}
void CPU::sub_vx_to_vy()
{
    m_V[0xF] = m_V[m_current_instruction.Y] > m_V[m_current_instruction.X];
    m_V[m_current_instruction.X] -= m_V[m_current_instruction.Y];
}
void CPU::jump_to_v0_nnn()
{
    m_PC = m_V[0] + m_current_instruction.NNN;
}
