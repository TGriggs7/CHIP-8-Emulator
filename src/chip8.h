#ifndef CHIP8_H
#define CHIP8_H

#include "stdint.h"

#define MEMSIZE 0x1000
#define RESERVED_ADDR 0x200
#define WINDOW_X_MAX 64
#define WINDOW_Y_MAX 32
#define NUM_KEYS 16
#define HEX_SPRITE_SIZE 5

class Chip8 {

public:
  uint8_t memory[MEMSIZE];
  uint8_t regs[16];

  uint8_t window[WINDOW_X_MAX][WINDOW_Y_MAX];

  uint16_t pc;  // program counter
  uint16_t sp;  // stack pointer
  uint16_t reg_I;
  uint8_t reg_sound;
  uint8_t reg_delay;

  uint16_t stack[16];
  bool key_down[NUM_KEYS];

  bool redraw;

  Chip8();
  void load(const char* filename);
  void run_cycle();
};

#endif // CHIP8_H
