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

  cycles = 0;
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

void Chip8::print_window() {
  for (int i = 0; i < WINDOW_Y_MAX * WINDOW_X_MAX; i++) {
    if (window[i]) {
      cout << "X";
    } else {
      cout << ".";
    }

    // cout << bool(window[i]);

    if (i % WINDOW_X_MAX == 0 && i > 0) {
      cout << "\n";
    }
  }
  cout << "\n\n\n";
}

void Chip8::run_cycle() {

  uint16_t opcode = (memory[pc] << 8) | memory[pc + 1];
  pc += 2;

  switch (opcode & 0xF000) {
    // reg setter
    case 0x6000: {
      uint8_t reg = (opcode & 0x0F00) >> 8;
      regs[reg] = opcode & 0x00FF;

      break;
    }
   
    // I setter
    case 0xA000: {
      reg_I = opcode & 0x0FFF;
      break;
    }

    // TODO: error check bounds
    // TODO: bit-wise not logical 
    case 0xD000: {
      uint8_t x_start = regs[(opcode & 0x0F00) >> 8];
      uint8_t y_start = regs[(opcode & 0x00F0) >> 4];
      uint8_t num_bytes = opcode & 0x000F;

      uint8_t x_coor;
      uint8_t y_coor;
      uint8_t mem_val;

      for (int i = 0; i < num_bytes; i++) {
        for (int j = 0; j < 8; j++) {
          x_coor = (x_start + ((i * 8 + j) % 8)) % WINDOW_X_MAX;
          y_coor = (y_start + ((i * 8 + j) / 8)) % WINDOW_Y_MAX;
          mem_val = memory[reg_I + i] & (0b1 << (7 - j));

          // set 0xF register if a pixel is being turned off
          if (mem_val && window[y_coor * WINDOW_X_MAX + x_coor]) {
            regs[0xF] = 1;
          }

          // set new pixel value
          window[y_coor * WINDOW_X_MAX + x_coor] ^= mem_val;

        }
      }
      break;
    }

    default:
      cout << "RUH ROH RAGGY! Unknown opcode: " << opcode << "\n";
      exit(0);
  }

  cycles++;
  cout << int(cycles) << " CYCLE DONE\n";
  print_window();
}















