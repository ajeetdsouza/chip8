#include "cpu.hpp"

#include <algorithm>
#include <array>
#include <fstream>
#include <iostream>
#include <string>

Cpu::Cpu()
    : beep(false),
      delay_timer(0),
      frame_buf_(),
      frame_buf(frame_buf_),
      i(0),
      keys(),
      mem(),
      pc(0x200),
      redraw(true),
      sound_timer(0),
      sp(0),
      stack(),
      v() {
  std::copy(font_set.begin(), font_set.end(), mem.begin() + 0x50);
}

void Cpu::reset() {
  beep = false;
  delay_timer = 0;

  for (auto &bs : frame_buf_) bs.reset();

  i = 0;
  keys.reset();

  std::fill(mem.begin(), mem.begin() + 0x50, 0);
  std::fill(mem.begin() + 0x50 + font_set.size(), mem.end(), 0);

  pc = 0x200;
  redraw = true;
  sound_timer = 0;
  sp = 0;

  std::fill(stack.begin(), stack.end(), 0);
  std::fill(v.begin(), v.end(), 0);
}

void Cpu::load(const std::string &fname) {
  reset();

  std::ifstream file(fname, std::ios::binary);
  std::copy(std::istreambuf_iterator<char>(file),
            std::istreambuf_iterator<char>(), mem.begin() + 0x200);
}

// Fetch one opcode
uint16_t Cpu::get_opcode() const {
  return (mem[pc] << 8) | mem[pc + 1];  // big-endian
}

// Fetch and run one opcode
void Cpu::run_opcode() {
  const auto opcode = get_opcode();
  run_opcode(opcode);
}

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
