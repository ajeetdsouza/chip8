#ifndef INCLUDE_CPU_H_
#define INCLUDE_CPU_H_

#include <array>
#include <bitset>
#include <string>

class Cpu {
private:
    std::array<std::bitset<64>, 32> frame_buf_; // frame buffer
    std::array<uint8_t, 4096> mem;              // addressable memory
    std::array<uint16_t, 16> stack;             // stack memory

    std::array<uint8_t, 16> v; // register V0 - VF
    uint16_t i;                // "I" register

    uint8_t sp;  // stack pointer
    uint16_t pc; // program counter

    uint8_t delay_timer;
    uint8_t sound_timer;

    static constexpr std::array<uint8_t, 16 * 5> font_set{
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


    inline uint16_t get_opcode() const;

    void run_opcode(uint16_t);

    void run_timers();

    void opcode_0x0NNN(uint16_t);

    void opcode_0x00EE(uint16_t);

    void opcode_0x00E0(uint16_t);

    void opcode_0x1NNN(uint16_t);

    void opcode_0x2NNN(uint16_t);

    void opcode_0x3XNN(uint16_t);

    void opcode_0x4XNN(uint16_t);

    void opcode_0x5XY0(uint16_t);

    void opcode_0x6XNN(uint16_t);

    void opcode_0x7XNN(uint16_t);

    void opcode_0x8XY0(uint16_t);

    void opcode_0x8XY1(uint16_t);

    void opcode_0x8XY2(uint16_t);

    void opcode_0x8XY3(uint16_t);

    void opcode_0x8XY4(uint16_t);

    void opcode_0x8XY5(uint16_t);

    void opcode_0x8XY6(uint16_t);

    void opcode_0x8XY7(uint16_t);

    void opcode_0x8XYE(uint16_t);

    void opcode_0x9XY0(uint16_t);

    void opcode_0xANNN(uint16_t);

    void opcode_0xBNNN(uint16_t);

    void opcode_0xCXNN(uint16_t);

    void opcode_0xDXYN(uint16_t);

    void opcode_0xEX9E(uint16_t);

    void opcode_0xEXA1(uint16_t);

    void opcode_0xFX07(uint16_t);

    void opcode_0xFX0A(uint16_t);

    void opcode_0xFX15(uint16_t);

    void opcode_0xFX18(uint16_t);

    void opcode_0xFX1E(uint16_t);

    void opcode_0xFX29(uint16_t);

    void opcode_0xFX33(uint16_t);

    void opcode_0xFX55(uint16_t);

    void opcode_0xFX65(uint16_t);

public:
    Cpu(const std::string &);

    const decltype(frame_buf_) &frame_buf; // read-only frame buffer

    void cycle();

    std::bitset<16> keys; // store key presses

    bool redraw;
    bool beep;
};

#endif // INCLUDE_CPU_H_
