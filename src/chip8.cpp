#include "chip8.h"
#include <string.h>
#include <stdio.h>
#include <iostream>

using namespace std;

Chip8::Chip8() {
  memset(memory, 0, 4096);
  memset(registers, 0, 16);

  pc = 512; // program starts at address 0x200
  sp = 4096; // set stack to top of memory
  reg_I = 0;

  memset(stack, 0, 16 * 2);
  memset(key_down, 0, 16);

  reg_sounds = 0;
  reg_delay = 0;
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

  // read it into memory
  fread(&memory, 1, sz, file);

  return true;
}
