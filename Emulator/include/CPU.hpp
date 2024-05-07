#pragma once
#include <SDL2/SDL.h>
#include <format>
#include "Config.hpp"
#include "Memory.hpp"
#include <stdio.h>
#include <string>
#include <iostream>
#include <stdint.h>

#define LOG(message) std::cout << "[DEBUG] " << message << std::endl
namespace Emulator
{

    typedef struct
    {
        uint16_t op;
        uint16_t NNN;
        uint8_t NN;
        uint8_t N;
        uint8_t X;
        uint8_t Y;

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
        SKIP_IF_VX_NEQ_VY,
        SET_I_TO_FONT_LOCATION,
        JUMP_NNN,
        JUMP_TO_V0_NNN,
        SET_I_TO_NNN,
        SET_VX_TO_NN,
        ADD_NN_TO_VX,
        SET_VX_TO_VY,
        STORE_BCD,
        VX_OR_VY,
        VX_AND_VY,
        VX_XOR_VY,
        ADD_VX_TO_VY,
        SUB_VX_TO_VY,
        SHIFT_VX_TO_RIGHT_BY_ONE,
        SHIFT_VX_TO_LEFT_BY_ONE,
        SET_VX_TO_SUB_VY,
        DRAW,
        ADD_VX_TO_I,
        SET_VX_TO_KEYPAD,
        LOAD_V_REGISTERS,
        DUMP_V_REGISTERS,
        SKIP_IF_KEY_PRESSED,
        SKIP_IF_KEY_NOT_PRESSED,
        SET_VX_TO_DELAY_TIMER,
        SET_SOUND_TIMER_TO_VX,
        SET_DELAY_TIMER_TO_VX,
        RANDOM,
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
        void set_vx_to_sub_vy();
        void shift_vx_to_right_by_one();
        void shift_vx_to_left_by_one();
        void display();
        void skip_if_vx_neq_vy();
        void load_v_registers();
        void dump_v_registers();
        void store_bcd();
        void random();
        void skip_if_key_pressed();
        void skip_if_key_not_pressed();
        void set_vx_to_delay_timer();
        void set_sound_timer_to_vx();
        void set_delay_timer_to_vx();
        void set_i_to_font_location();

    public:
        bool KEY_PAD[16]={false};
        bool draw;
        OP_CODE decode();
        void execute(OP_CODE op);
        void fetch();
        void dec_delay_timer();
        bool dec_sound_timer();
        CPU(Memory &memory, Config &config);
        ~CPU();
    };

} // namespace Emulator
