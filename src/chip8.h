#ifndef CHIP8_H
#define CHIP8_H

#include "stdint.h"

#define MEMSIZE 0x1000
#define RESERVED_ADDR 0x200

class Chip8 {

public:
  uint8_t memory[4096];
  uint8_t registers[16];

  uint16_t pc;  // program counter
  uint16_t sp;  // stack pointer
  uint16_t reg_I;

  uint16_t stack[16];
  bool key_down[16];

  uint8_t reg_sounds;
  uint8_t reg_delay;

  Chip8();
  bool load(const char* filename);
  void start();
};

#endif // CHIP8_H
