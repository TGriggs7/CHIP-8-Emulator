#include "chip8.h"
#include <string.h>
#include <stdio.h>
#include <iostream>

using namespace std;

Chip8::Chip8() {
  memset(memory, 0, MEMSIZE);
  memset(regs, 0, 16);

  memset(window, 0, WINDOW_X_MAX * WINDOW_Y_MAX);

  pc = RESERVED_ADDR; // program starts at address 0x200
  sp = MEMSIZE; // set stack to top of memory
  reg_I = 0;

  memset(stack, 0, 16 * 2);
  memset(key_down, 0, 16);

  reg_sounds = 0;
  reg_delay = 0;

  // sprite data of each digit 0-F is stored in reserved memory 0x000-0x1FF
  uint8_t hex_digits[80] = {
    0xF0, 0x90, 0x90, 0x90, 0xF0,
    0x20, 0x60, 0x20, 0x20, 0x70,
    0xF0, 0x10, 0xF0, 0x80, 0xF0,
    0xF0, 0x10, 0xF0, 0x10, 0xF0,
    0x90, 0x90, 0xF0, 0x10, 0x10,
    0xF0, 0x80, 0xF0, 0x10, 0xF0,
    0xF0, 0x80, 0xF0, 0x90, 0xF0,
    0xF0, 0x10, 0x20, 0x40, 0x40,
    0xF0, 0x90, 0xF0, 0x90, 0xF0,
    0xF0, 0x90, 0xF0, 0x10, 0xF0,
    0xF0, 0x90, 0xF0, 0x90, 0x90,
    0xE0, 0x90, 0xE0, 0x90, 0xE0,
    0xF0, 0x80, 0x80, 0x80, 0xF0,
    0xE0, 0x90, 0x90, 0x90, 0xE0,
    0xF0, 0x80, 0xF0, 0x80, 0xF0,
    0xF0, 0x80, 0xF0, 0x80, 0x80 
  };

  memcpy(memory, hex_digits, 80);
}

bool Chip8::load(const char* filename) {
  FILE* file;

  // open file
  file = fopen(filename, "r");
  if (!file) {
    cout << "RUH ROH RAGGY! Bad filename\n";
    return false;
  }

  // check file size
  fseek(file, 0L, SEEK_END);
  uint32_t sz = ftell(file);
  rewind(file);
  if (sz > (MEMSIZE - RESERVED_ADDR)) {
    cout << "RUH ROH RAGGY! File too large\n";
    return false;
  }

  // read rom into memory
  fread(&memory[RESERVED_ADDR], 1, sz, file);

  return true;
}

void Chip8::run_cycle() {

  uint16_t opcode = (memory[pc] << 8) | memory[pc];
  pc += 2;

  switch (opcode & 0xF000) {
    case 0x0000:
    case 0x1000:
    case 0x2000:
    case 0x3000:
    case 0x4000:
    case 0x5000:
      break;

    // reg setter
    case 0x6000: {
      uint8_t reg = (opcode & 0xF000) >> 16;
      regs[reg] = opcode & 0x00FF;
      break;
    }

    case 0x7000:
    case 0x8000:
    case 0x9000:

    // I setter
    case 0xA000: {
      reg_I = opcode & 0x0FFF;
      break;
    }


    case 0xB000:
    case 0xC000:

    // Dxyn -> n byte sprite at address I to be drawn at (Vx, Vy), sprite ix XORed, set Vf if erases pixels, sprite wraps
    case 0xD000: {
      
    }


    case 0xE000:
    case 0xF000:
    default:
      cout << "RUH ROH RAGGY! Unknown opcode: " << opcode << "\n";
      exit(0);
  }

}















