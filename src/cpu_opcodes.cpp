#include "cpu.hpp"

#define X ((opcode & 0x0F00) >> 8)
#define Y ((opcode & 0x00F0) >> 4)
#define N (opcode & 0x000F)

#define NN (opcode & 0x00FF)
#define NNN (opcode & 0x0FFF)

/* Calls RCA 1802 program at address NNN.
 * Not necessary for most ROMs. */
void Cpu::opcode_0x0NNN(const uint16_t opcode = 0x0000) { pc += 2; }

/* Clears the screen. */
void Cpu::opcode_0x00E0(const uint16_t opcode = 0x00E0) {
  frame_buf_ = {};
  pc += 2;
}

/* Returns from a subroutine. */
void Cpu::opcode_0x00EE(const uint16_t opcode = 0x00EE) { pc = stack[--sp]; }

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
void Cpu::opcode_0x3XNN(const uint16_t opcode) { pc += v[X] == NN ? 4 : 2; }

/* Skips the next instruction if VX doesn't equal NN. */
void Cpu::opcode_0x4XNN(const uint16_t opcode) { pc += v[X] != NN ? 4 : 2; }

/* Skips the next instruction if VX equals VY. */
void Cpu::opcode_0x5XY0(const uint16_t opcode) { pc += v[X] == v[Y] ? 4 : 2; }

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
  v[0xF] = static_cast<uint8_t>(__builtin_add_overflow(v[X], v[Y], &v[X]));
  pc += 2;
}

/* VY is subtracted from VX.
 * VF is set to 0 when there's a borrow, and 1 when there isn't. */
void Cpu::opcode_0x8XY5(const uint16_t opcode) {
  v[0xF] = static_cast<uint8_t>(!__builtin_sub_overflow(v[X], v[Y], &v[X]));
  pc += 2;
}

/* Stores the least significant bit of VX in VF and then shifts VX to the right
 * by 1. */
void Cpu::opcode_0x8XY6(const uint16_t opcode) {
  v[0xF] = static_cast<uint8_t>(v[X] & 0x01);
  v[X] >>= 1;
  pc += 2;
}

/* Sets VX to VY minus VX.
 * VF is set to 0 when there's a borrow, and 1 when there isn't. */
void Cpu::opcode_0x8XY7(const uint16_t opcode) {
  v[0xF] = static_cast<uint8_t>(!__builtin_sub_overflow(v[Y], v[X], &v[X]));
  pc += 2;
}

/* Stores the most significant bit of VX in VF and then shifts VX to the left
 * by 1. */
void Cpu::opcode_0x8XYE(const uint16_t opcode) {
  v[0xF] = static_cast<uint8_t>((v[X] & 0x80) >> 7);
  v[X] <<= 1;
  pc += 2;
}

/* Skips the next instruction if VX doesn't equal VY. */
void Cpu::opcode_0x9XY0(const uint16_t opcode) { pc += v[X] != v[Y] ? 4 : 2; }

/* Sets I to the address NNN. */
void Cpu::opcode_0xANNN(const uint16_t opcode) {
  i = static_cast<uint16_t>(NNN);
  pc += 2;
}

/* Jumps to the address NNN plus V0. */
void Cpu::opcode_0xBNNN(const uint16_t opcode) {
  pc = static_cast<uint16_t>(NNN + v[0]);
}

/* Sets VX to the result of a bitwise AND operation on a random number and NN.
 */
void Cpu::opcode_0xCXNN(const uint16_t opcode) {
  const auto r = rand();
  v[X] = static_cast<uint8_t>(r & NN);
  pc += 2;
}

/* Draws a sprite at coordinate (VX, VY).
 * The sprite has a width of 8 px and a height of N px.
 * Each row of 8 pixels is read as bit-coded starting from memory location I.
 * I value doesn’t change after the execution of this instruction.
 * VF is set to 1 if any screen pixels are flipped from set to unset when the
 * sprite is drawn, and to 0 if that doesn’t happen */
void Cpu::opcode_0xDXYN(const uint16_t opcode) {
  v[0xF] = 0;
  for (auto y = 0; y < N; ++y) {
    const auto sprite_ln = mem[i + y];

    for (auto x = 0; x < 8; ++x) {
      const auto sprite_px = (sprite_ln >> (7 - x)) & 1;
      const auto screen_px = frame_buf_[v[Y] + y][v[X] + x];

      v[0xF] |= screen_px & sprite_px;  // collision

      frame_buf_[v[Y] + y][v[X] + x] = screen_px ^ sprite_px;
    }
  }
  redraw = true;
  pc += 2;
}

/* Skips the next instruction if the key stored in VX is pressed. */
void Cpu::opcode_0xEX9E(const uint16_t opcode) { pc += keys[v[X]] ? 4 : 2; }

/* Skips the next instruction if the key stored in VX isn't pressed. */
void Cpu::opcode_0xEXA1(const uint16_t opcode) { pc += !keys[v[X]] ? 4 : 2; }

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
 * The offset from I is increased by 1 for each value written, but I itself is
 * left unmodified. */
void Cpu::opcode_0xFX55(const uint16_t opcode) {
  for (auto x_i = 0; x_i <= X; ++x_i) mem[i + x_i] = v[x_i];
  pc += 2;
}

/* Fills V0 to VX (including VX) with values from memory starting at address I.
 * The offset from I is increased by 1 for each value written, but I itself is
 * left unmodified. */
void Cpu::opcode_0xFX65(const uint16_t opcode) {
  for (auto x_i = 0; x_i <= X; ++x_i) v[x_i] = mem[i + x_i];
  pc += 2;
}
