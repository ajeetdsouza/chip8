#include "cpu.hpp"

#include <algorithm>
#include <array>
#include <fstream>
#include <iostream>
#include <string>

#define X ((opcode & 0x0F00) >> 8)
#define Y ((opcode & 0x00F0) >> 4)
#define N (opcode & 0x000F)

#define NN (opcode & 0x00FF)
#define NNN (opcode & 0x0FFF)

Cpu::Cpu(const std::string &fname)
        : beep(false),
          delay_timer(),
          frame_buf_(),
          frame_buf(frame_buf_),
          i(0),
          keys(),
          mem(),
          pc(0x200),
          redraw(true),
          sound_timer(),
          sp(0),
          stack(),
          v() {
    std::copy(font_set.begin(), font_set.end(), mem.begin() + 0x50);
    std::ifstream file(fname, std::ios::binary);
    std::copy(std::istreambuf_iterator<char>(file),
              std::istreambuf_iterator<char>(), mem.begin() + 0x200);
}

// Run one cycle
void Cpu::cycle() {
    const auto opcode = get_opcode();

    run_opcode(opcode);
    run_timers();
}

// Fetch one opcode
uint16_t Cpu::get_opcode() const {
    return (mem[pc] << 8) | mem[pc + 1]; // big-endian
}

// Fetch and run one opcode
void Cpu::run_opcode(const uint16_t opcode) {
    switch (opcode & 0xF000) {
        case 0x0000:
            switch (opcode) {
                case 0x00E0:
                    return opcode_0x00E0(opcode);
                case 0x00EE:
                    return opcode_0x00EE(opcode);
                default:
                    return opcode_0x0NNN(opcode);
            }

        case 0x1000:
            return opcode_0x1NNN(opcode);

        case 0x2000:
            return opcode_0x2NNN(opcode);

        case 0x3000:
            return opcode_0x3XNN(opcode);

        case 0x4000:
            return opcode_0x4XNN(opcode);

        case 0x5000:
            if ((opcode & 0xF) == 0)
                return opcode_0x5XY0(opcode);
            else
                break;

        case 0x6000:
            return opcode_0x6XNN(opcode);

        case 0x7000:
            return opcode_0x7XNN(opcode);

        case 0x8000:
            switch (opcode & 0xF) {
                case 0x0:
                    return opcode_0x8XY0(opcode);
                case 0x1:
                    return opcode_0x8XY1(opcode);
                case 0x2:
                    return opcode_0x8XY2(opcode);
                case 0x3:
                    return opcode_0x8XY3(opcode);
                case 0x4:
                    return opcode_0x8XY4(opcode);
                case 0x5:
                    return opcode_0x8XY5(opcode);
                case 0x6:
                    return opcode_0x8XY6(opcode);
                case 0x7:
                    return opcode_0x8XY7(opcode);
                case 0xE:
                    return opcode_0x8XYE(opcode);
                default:
                    break;
            }
            break;

        case 0x9000:
            if ((opcode & 0xF) == 0)
                return opcode_0x9XY0(opcode);
            else
                break;

        case 0xA000:
            return opcode_0xANNN(opcode);

        case 0xB000:
            return opcode_0xBNNN(opcode);

        case 0xC000:
            return opcode_0xCXNN(opcode);

        case 0xD000:
            return opcode_0xDXYN(opcode);

        case 0xE000:
            switch (opcode & 0xFF) {
                case 0x9E:
                    return opcode_0xEX9E(opcode);
                case 0xA1:
                    return opcode_0xEXA1(opcode);
                default:
                    break;
            }
            break;

        case 0xF000:
            switch (opcode & 0xFF) {
                case 0x07:
                    return opcode_0xFX07(opcode);
                case 0x0A:
                    return opcode_0xFX0A(opcode);
                case 0x15:
                    return opcode_0xFX15(opcode);
                case 0x18:
                    return opcode_0xFX18(opcode);
                case 0x1E:
                    return opcode_0xFX1E(opcode);
                case 0x29:
                    return opcode_0xFX29(opcode);
                case 0x33:
                    return opcode_0xFX33(opcode);
                case 0x55:
                    return opcode_0xFX55(opcode);
                case 0x65:
                    return opcode_0xFX65(opcode);
                default:
                    break;
            }
            break;
        default:
            break;
    }

    std::cerr << "Invalid opcode: " << std::showbase << std::hex << opcode
              << std::noshowbase << std::dec << std::endl;
}

// Run timers for one cycle
void Cpu::run_timers() {
    if (delay_timer > 0) --delay_timer;

    if (sound_timer > 0) {
        beep = true;
        --sound_timer;
    }
}

/* Calls RCA 1802 program at address NNN.
 * Not necessary for most ROMs. */
void Cpu::opcode_0x0NNN(const uint16_t opcode = 0x0000) { pc += 2; }

/* Clears the screen. */
void Cpu::opcode_0x00E0(const uint16_t opcode = 0x00E0) {
    frame_buf_ = {};
    pc += 2;
}

/* Returns from a subroutine. */
void Cpu::opcode_0x00EE(const uint16_t opcode = 0x00EE) {
    pc = stack[--sp];
}

/* Jumps to address NNN. */
void Cpu::opcode_0x1NNN(const uint16_t opcode) {
    pc = static_cast<uint16_t>(NNN);
}

/* Calls subroutine at NNN. */
void Cpu::opcode_0x2NNN(const uint16_t opcode) {
    stack[sp++] = static_cast<uint16_t>(pc + 2);
    pc = static_cast<uint16_t>(NNN);
}

/* Skips the next instruction if VX equals NN. */
void Cpu::opcode_0x3XNN(const uint16_t opcode) {
    if (v[X] == NN)
        pc += 4;
    else
        pc += 2;
}

/* Skips the next instruction if VX doesn't equal NN. */
void Cpu::opcode_0x4XNN(const uint16_t opcode) {
    if (v[X] != NN)
        pc += 4;
    else
        pc += 2;
}

/* Skips the next instruction if VX equals VY. */
void Cpu::opcode_0x5XY0(const uint16_t opcode) {
    if (v[X] == v[Y])
        pc += 4;
    else
        pc += 2;
}

/* Sets VX to NN. */
void Cpu::opcode_0x6XNN(const uint16_t opcode) {
    v[X] = static_cast<uint8_t>(NN);
    pc += 2;
}

/* Adds NN to VX.
 * Carry flag is not changed. */
void Cpu::opcode_0x7XNN(const uint16_t opcode) {
    v[X] += NN;
    pc += 2;
}

/* Sets VX to the value of VY. */
void Cpu::opcode_0x8XY0(const uint16_t opcode) {
    v[X] = v[Y];
    pc += 2;
}

/* Sets VX to VX OR VY. */
void Cpu::opcode_0x8XY1(const uint16_t opcode) {
    v[X] |= v[Y];
    pc += 2;
}

/* Sets VX to VX AND VY. */
void Cpu::opcode_0x8XY2(const uint16_t opcode) {
    v[X] &= v[Y];
    pc += 2;
}

/* Sets VX to VX XOR VY. */
void Cpu::opcode_0x8XY3(const uint16_t opcode) {
    v[X] ^= v[Y];
    pc += 2;
}

/* Adds VY to VX.
 * VF is set to 1 when there's a carry, and to 0 when there isn't. */
void Cpu::opcode_0x8XY4(const uint16_t opcode) {
    if (__builtin_add_overflow(v[X], v[Y], &v[X]))
        v[0xF] = 1;
    else
        v[0xF] = 0;
    pc += 2;
}

/* VY is subtracted from VX.
 * VF is set to 0 when there's a borrow, and 1 when there isn't. */
void Cpu::opcode_0x8XY5(const uint16_t opcode) {
    if (__builtin_sub_overflow(v[X], v[Y], &v[X]))
        v[0xF] = 0;
    else
        v[0xF] = 1;
    pc += 2;
}

/* Stores the least significant bit of VX in VF and then shifts VX to the right by 1. */
void Cpu::opcode_0x8XY6(const uint16_t opcode) {
    v[0xF] = static_cast<uint8_t>(v[X] & 0x01);
    v[X] >>= 1;
    pc += 2;
}

/* Sets VX to VY minus VX.
 * VF is set to 0 when there's a borrow, and 1 when there isn't. */
void Cpu::opcode_0x8XY7(const uint16_t opcode) {
    if (__builtin_sub_overflow(v[Y], v[X], &v[X]))
        v[0xF] = 0;
    else
        v[0xF] = 1;
    pc += 2;
}

/* Stores the most significant bit of VX in VF and then shifts VX to the left by 1. */
void Cpu::opcode_0x8XYE(const uint16_t opcode) {
    v[0xF] = static_cast<uint8_t >((v[X] & 0x80) >> 7);
    v[X] <<= 1;
    pc += 2;
}

/* Skips the next instruction if VX doesn't equal VY. */
void Cpu::opcode_0x9XY0(const uint16_t opcode) {
    if (v[X] != v[Y])
        pc += 4;
    else
        pc += 2;
}

/* Sets I to the address NNN. */
void Cpu::opcode_0xANNN(const uint16_t opcode) {
    i = static_cast<uint16_t >(NNN);
    pc += 2;
}

/* Jumps to the address NNN plus V0. */
void Cpu::opcode_0xBNNN(const uint16_t opcode) {
    pc = static_cast<uint16_t>(NNN + v[0]);
}

/* Sets VX to the result of a bitwise AND operation on a random number and NN. */
void Cpu::opcode_0xCXNN(const uint16_t opcode) {
    const auto r = rand();
    v[X] = static_cast<uint8_t>(r & NN);
    pc += 2;
}

/* Draws a sprite at coordinate (VX, VY).
 * The sprite has a width of 8 px and a height of N px.
 * Each row of 8 pixels is read as bit-coded starting from memory location I.
 * I value doesn’t change after the execution of this instruction.
 * VF is set to 1 if any screen pixels are flipped from set to unset when the sprite is drawn,
 * and to 0 if that doesn’t happen */
void Cpu::opcode_0xDXYN(const uint16_t opcode) {
    v[0xF] = 0;
    for (auto y = 0; y < N; ++y) {
        const auto sprite_ln = mem[i + y];

        for (auto x = 0; x < 8; ++x) {
            const auto sprite_px = (sprite_ln >> (7 - x)) & 1;
            const auto screen_px = frame_buf_[v[Y] + y][v[X] + x];

            if (screen_px & sprite_px)
                v[0xF] = 1; // collision

            frame_buf_[v[Y] + y][v[X] + x] = screen_px ^ sprite_px;
        }
    }
    redraw = true;
    pc += 2;
}

/* Skips the next instruction if the key stored in VX is pressed. */
void Cpu::opcode_0xEX9E(const uint16_t opcode) {
    if (keys[v[X]])
        pc += 4;
    else
        pc += 2;
}

/* Skips the next instruction if the key stored in VX isn't pressed. */
void Cpu::opcode_0xEXA1(const uint16_t opcode) {
    if (keys[v[X]])
        pc += 2;
    else
        pc += 4;
}

/* Sets VX to the value of the delay timer. */
void Cpu::opcode_0xFX07(const uint16_t opcode) {
    v[X] = delay_timer;
    pc += 2;
}

/* A key press is awaited, and then stored in VX.
 * This is a blocking operation.
 * All instructions are halted until the next key event. */
void Cpu::opcode_0xFX0A(const uint16_t opcode) {
    for (auto keys_i = 0; keys_i < 0xF; ++keys_i) {
        if (keys[keys_i]) {
            v[X] = static_cast<uint8_t>(keys_i);
            pc += 2;
            return;
        }
    }
}

/* Sets the delay timer to VX. */
void Cpu::opcode_0xFX15(const uint16_t opcode) {
    delay_timer = v[X];
    pc += 2;
}

/* Sets the sound timer to VX. */
void Cpu::opcode_0xFX18(const uint16_t opcode) {
    sound_timer = v[X];
    pc += 2;
}

/* Adds VX to I. */
void Cpu::opcode_0xFX1E(const uint16_t opcode) {
    i += v[X];
    pc += 2;
}

/* Sets I to the location of the sprite for the character in VX.
 * Characters 0-F (in hexadecimal) are represented by a 4x5 font. */
void Cpu::opcode_0xFX29(const uint16_t opcode) {
    i = static_cast<uint16_t>(0x50 + v[X] * 5);
    pc += 2;
}

/* Stores the binary-coded decimal representation of VX.
 * 100s place is stored at I.
 *  10s place is stored at I + 1.
 *   1s place is stored at I + 2. */
void Cpu::opcode_0xFX33(const uint16_t opcode) {
    mem[i] = static_cast<uint8_t>(v[X] / 100);
    mem[i + 1] = static_cast<uint8_t>((v[X] / 10) % 10);
    mem[i + 2] = static_cast<uint8_t>(v[X] % 10);
    pc += 2;
}

/* Stores V0 to VX (including VX) in memory starting at address I.
 * The offset from I is increased by 1 for each value written, but I itself is left unmodified. */
void Cpu::opcode_0xFX55(const uint16_t opcode) {
    for (auto x_i = 0; x_i <= X; ++x_i) mem[i + x_i] = v[x_i];
    pc += 2;
}

/* Fills V0 to VX (including VX) with values from memory starting at address I.
 * The offset from I is increased by 1 for each value written, but I itself is left unmodified. */
void Cpu::opcode_0xFX65(const uint16_t opcode) {
    for (auto x_i = 0; x_i <= X; ++x_i) v[x_i] = mem[i + x_i];
    pc += 2;
}
