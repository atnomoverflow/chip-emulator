#include "CPU.hpp"

namespace Emulator
{

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
                break;
                // return EXECUTE_SUB_NNN;
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
            case 0x6:
                return SHIFT_VX_TO_RIGHT_BY_ONE;
            case 0x7:
                return SET_VX_TO_SUB_VY;
            case 0xE:
                return SHIFT_VX_TO_LEFT_BY_ONE;
            default:
                break;
            }
        case 0x9:
            return SKIP_IF_VX_NEQ_VY;
        case 0x0A:
            return SET_I_TO_NNN;
        case 0x0B:
            return JUMP_TO_V0_NNN;
        case 0x0C:
            return RANDOM;
        case 0x0D:
            return DRAW;
        case 0x0E:
            switch (m_current_instruction.NN)
            {
            case 0x9E:
            {
                return SKIP_IF_KEY_PRESSED;
            }
            case 0xA1:
            {
                return SKIP_IF_KEY_NOT_PRESSED;
            }
            default:
                break;
            }
        case 0x0F:
            switch (m_current_instruction.NN)
            {
            case 0x07:
                return SET_VX_TO_DELAY_TIMER;
            case 0x0A:
                return SET_VX_TO_KEYPAD;
            case 0x15:
                return SET_DELAY_TIMER_TO_VX;
            case 0x18:
                return SET_SOUND_TIMER_TO_VX;
            case 0x1E:
                return ADD_VX_TO_I;
            case 0x29:
                return SET_I_TO_FONT_LOCATION;
            case 0x33:
                return STORE_BCD;
            case 0x55:
                return DUMP_V_REGISTERS;
            case 0x65:
                return LOAD_V_REGISTERS;
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
            // DXYN
            LOG(std::format("op: 0x{:04X},address: 0x{:04X}, Desc: Display V{}: 0x{:04X} V{}: 0x{:04X} N: 0x{:04X} I:0x{:04X}  \n", m_current_instruction.op, m_PC, m_current_instruction.X, m_V[m_current_instruction.X], m_current_instruction.Y, m_V[m_current_instruction.Y], m_current_instruction.N, m_I));
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
        case SHIFT_VX_TO_RIGHT_BY_ONE:
        {
            LOG(std::format("op: 0x{:04X},address: 0x{:04X}, Desc: Shift V{}: 0x{:04X} to the right \n", m_current_instruction.op, m_PC, m_current_instruction.X, m_V[m_current_instruction.X]));
            shift_vx_to_right_by_one();
            break;
        }
        case SHIFT_VX_TO_LEFT_BY_ONE:
        {
            LOG(std::format("op: 0x{:04X},address: 0x{:04X}, Desc: Shift V{}: 0x{:04X} to the left \n", m_current_instruction.op, m_PC, m_current_instruction.X, m_V[m_current_instruction.X]));
            shift_vx_to_left_by_one();
            break;
        }
        case SET_VX_TO_SUB_VY:
        {
            LOG(std::format("op: 0x{:04X},address: 0x{:04X}, Desc: Set V{} = V{} ({:04X}) -V{} ({:04X})  \n", m_current_instruction.op, m_PC, m_current_instruction.X, m_current_instruction.X, m_V[m_current_instruction.X], m_current_instruction.Y, m_V[m_current_instruction.Y]));
            set_vx_to_sub_vy();
            break;
        }
        case SKIP_IF_VX_NEQ_VY:
        {
            LOG(std::format("op: 0x{:04X},address: 0x{:04X}, Desc: Skip if V{} ({:04X}) != V{} ({:04X})  \n", m_current_instruction.op, m_PC, m_current_instruction.X, m_V[m_current_instruction.X], m_current_instruction.Y, m_V[m_current_instruction.Y]));
            skip_if_vx_neq_vy();
            break;
        }
        case LOAD_V_REGISTERS:
        {
            LOG(std::format("op: 0x{:04X},address: 0x{:04X}, Desc: Load V register from ram starting from I :{:04X} \n", m_current_instruction.op, m_PC, m_I));
            load_v_registers();
            break;
        }
        case DUMP_V_REGISTERS:
        {
            LOG(std::format("op: 0x{:04X},address: 0x{:04X}, Desc: Dump V register to ram starting from I :{:04X} \n", m_current_instruction.op, m_PC, m_I));
            dump_v_registers();
            break;
        }
        case STORE_BCD:
        {
            LOG(std::format("op: 0x{:04X},address: 0x{:04X}, Desc: store bcd of v{}:{:04X} starting from I :{:04X} \n", m_current_instruction.op, m_PC, m_current_instruction.X, m_V[m_current_instruction.X], m_I));
            store_bcd();
            break;
        }
        case RANDOM:
        {
            LOG(std::format("op: 0x{:04X},address: 0x{:04X}, Desc: Set V{} to rand() & NN: {:04X} \n", m_current_instruction.op, m_PC, m_current_instruction.X, m_current_instruction.NN));
            random();
            break;
        }
        case SKIP_IF_KEY_PRESSED:
        {
            LOG(std::format("op: 0x{:04X},address: 0x{:04X}, Desc: Skip if key() == V{} {:04X} \n", m_current_instruction.op, m_PC, m_current_instruction.X, m_V[m_current_instruction.X]));
            skip_if_key_pressed();
            break;
        }
        case SKIP_IF_KEY_NOT_PRESSED:
        {
            LOG(std::format("op: 0x{:04X},address: 0x{:04X}, Desc: Skip if key() != V{} {:04X} \n", m_current_instruction.op, m_PC, m_current_instruction.X, m_V[m_current_instruction.X]));
            skip_if_key_not_pressed();
            break;
        }
        case SET_VX_TO_DELAY_TIMER:
        {
            LOG(std::format("op: 0x{:04X},address: 0x{:04X}, Desc: Set V{} to delay timer \n", m_current_instruction.op, m_PC, m_current_instruction.X));
            set_vx_to_delay_timer();
            break;
        }
        case SET_DELAY_TIMER_TO_VX:
        {
            LOG(std::format("op: 0x{:04X},address: 0x{:04X}, Desc: Set delay timer to V{} \n", m_current_instruction.op, m_PC, m_current_instruction.X));
            set_delay_timer_to_vx();
            break;
        }
        case SET_SOUND_TIMER_TO_VX:
        {
            LOG(std::format("op: 0x{:04X},address: 0x{:04X}, Desc: Set sound timer to V{} \n", m_current_instruction.op, m_PC, m_current_instruction.X));
            set_sound_timer_to_vx();
            break;
        }
        case SET_I_TO_FONT_LOCATION:
        {
            LOG(std::format("op: 0x{:04X},address: 0x{:04X}, Desc: Set I {:04X} to sprite location V{} \n", m_current_instruction.op, m_PC, m_I, m_current_instruction.X));
            set_i_to_font_location();
            break;
        }
        default:
            LOG(std::format("op: 0x{:04X},address: 0x{:04X}, Desc: UNIMPLIMENTED", m_current_instruction.op, m_PC));
            break;
        }
    }

    // op codes logic

    void CPU::clear_screen()
    {
        for (size_t i = 0; i < m_memory.display_size; i++)
        {
            m_memory.DISPLAY[i] = false;
        }
        draw = true;
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
        static uint8_t key = 0xFF;

        if (key < 16)
        {
            for (uint8_t i = 0; i < 16; ++i)
            {
                if (KEY_PAD[i])
                {
                    key = i;
                    return;
                }
            }
            m_PC -= 2;
        }
        else
        {
            if (KEY_PAD[key])
            {
                m_PC -= 2;
            }
            else
            {
                m_V[m_current_instruction.X] = key;
                key = 0xFF;
            }
        }
    }
    void CPU::call_sub_at_nnn()
    {
        *m_SP++ = m_PC;
        m_PC = m_current_instruction.NNN;
    }
    void CPU::display()
    {
        int x = (uint16_t)m_V[m_current_instruction.X] % m_config.WIDTH;
        int y = (uint16_t)m_V[m_current_instruction.Y] % m_config.HEIGHT;
        int orig_x = x;
        m_V[0xF] = 0; // VF carry flag

        for (uint8_t i = 0; i < m_current_instruction.N; i++)
        {
            // get sprit data from ram
            const uint8_t sprit = m_memory.get_data((m_I + i));
            x = orig_x;
            for (int8_t j = 7; j >= 0; j--)
            {

                bool *pixel = &m_memory.DISPLAY[y * m_config.WIDTH + x];
                const bool sprit_pixel = (sprit & (1 << j));
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
        draw = true;
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
        bool carry = ((uint16_t)(m_V[m_current_instruction.Y] + m_V[m_current_instruction.X]) > 255);
        m_V[m_current_instruction.X] += m_V[m_current_instruction.Y];
        m_V[0xF] = carry;
    }
    void CPU::sub_vx_to_vy()
    {
        bool carry = (m_V[m_current_instruction.Y] <= m_V[m_current_instruction.X]);
        m_V[m_current_instruction.X] -= m_V[m_current_instruction.Y];
        m_V[0xF] = carry;
    }
    void CPU::jump_to_v0_nnn()
    {
        m_PC = m_V[0] + m_current_instruction.NNN;
    }
    void CPU::shift_vx_to_right_by_one()
    {
        bool carry = m_V[m_current_instruction.X] & 1;
        m_V[m_current_instruction.X] >>= 1;
        m_V[0xF] = carry;
    }
    void CPU::shift_vx_to_left_by_one()
    {
        bool carry = (m_V[m_current_instruction.X] >> 7) & 1;
        m_V[m_current_instruction.X] <<= 1;
        m_V[0xF] = carry;
    }
    void CPU::set_vx_to_sub_vy()
    {
        bool carry = (m_V[m_current_instruction.Y] >= m_V[m_current_instruction.X]);
        m_V[m_current_instruction.X] = m_V[m_current_instruction.Y] - m_V[m_current_instruction.X];
        m_V[0xF] = carry;
    }
    void CPU::skip_if_vx_neq_vy()
    {
        if (m_V[m_current_instruction.Y] != m_V[m_current_instruction.X])
        {
            m_PC += 2;
        }
    }

    void CPU::load_v_registers()
    {
        for (uint8_t i = 0; i <= m_current_instruction.X; i++)
        {
            m_V[i] = m_memory.get_data(m_I + i);
        }
    }
    void CPU::dump_v_registers()
    {
        for (uint8_t i = 0; i <= m_current_instruction.X; i++)
        {
            m_memory.set_data(m_I + i, m_V[i]);
        }
    }
    void CPU::store_bcd()
    {
        uint8_t bcd = m_V[m_current_instruction.X];
        m_memory.set_data(m_I + 2, bcd % 10);
        bcd /= 10;
        m_memory.set_data(m_I + 1, bcd % 10);
        bcd /= 10;
        m_memory.set_data(m_I, bcd);
    }
    void CPU::random()
    {
        m_V[m_current_instruction.X] = ((uint8_t)(rand() % 256)) & m_current_instruction.NN;
    }
    void CPU::skip_if_key_pressed()
    {
        // inc PC by 2 if key pressed is == VX
        if (KEY_PAD[m_V[m_current_instruction.X]])
        {
            m_PC += 2;
        }
    }
    void CPU::skip_if_key_not_pressed()
    {
        // inc PC by 2 if key pressed is != VX
        if (!KEY_PAD[m_V[m_current_instruction.X]])
        {
            m_PC += 2;
        }
    }
    void CPU::set_vx_to_delay_timer()
    {
        m_V[m_current_instruction.X] = m_delay_timer;
    }
    void CPU::set_sound_timer_to_vx()
    {
        m_V[m_current_instruction.X] = m_sound_timer;
    }
    void CPU::set_delay_timer_to_vx()
    {
        m_V[m_current_instruction.X] = m_delay_timer;
    }
    void CPU::set_i_to_font_location()
    {
        m_I = m_V[m_current_instruction.X] * 5;
    }

    // input handling logic
    void CPU::dec_delay_timer()
    {
        if (m_delay_timer > 0)
            m_delay_timer--;
    }
    bool CPU::dec_sound_timer()
    {
        if (m_sound_timer > 0)
        {
            m_sound_timer--;
            return true;
        }
        return false;
    }

} // namespace Emulator
