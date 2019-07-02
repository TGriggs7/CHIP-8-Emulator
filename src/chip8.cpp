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
  sp = 0; // set stack to top of memory
  reg_I = 0;

  memset(stack, 0, 16 * 2);
  memset(key_down, 0, NUM_KEYS);

  reg_sound = 0;
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

  redraw = false;
}

void Chip8::load(const char* filename) {
  FILE* file;

  // open file
  file = fopen(filename, "r");
  if (!file) {
    cout << "RUH ROH RAGGY! Bad filename\n";
    exit(2);
  }

  // check file size
  fseek(file, 0L, SEEK_END);
  uint32_t sz = ftell(file);
  rewind(file);
  if (sz > (MEMSIZE - RESERVED_ADDR)) {
    cout << "RUH ROH RAGGY! File too large\n";
    exit(2);
  }

  // read rom into memory
  fread(&memory[RESERVED_ADDR], 1, sz, file);

  fclose(file);
}

void Chip8::print_window() {
  for (int i = 0; i < WINDOW_Y_MAX; i++) {
    for (int j = 0; j < WINDOW_X_MAX; j++) {
      if (window[j][i]) {
        cout << "X";
      } else {
        cout << ".";
      }
    } 
    cout << "\n";
  }
  cout << "\n\n\n";
}

void Chip8::run_cycle() {

  uint16_t opcode = (memory[pc] << 8) | memory[pc + 1];
  pc += 2;

  switch (opcode & 0xF000) {
    // TODO: error check stack pointer
    case 0x0000: {
      switch (opcode & 0x00FF) {
        case 0x00E0: {
          memset(window, 0, WINDOW_X_MAX * WINDOW_Y_MAX);
          redraw = true;
          break;
        }
        // return from subroutine
        case 0x00EE: {
          pc = stack[--sp];
          break;
        }

        default:
          cout << "RUH ROH RAGGY! Unknown opcode: " << int(opcode) << "\n";
          exit(0);
      }
      break;
    }

    // 1nnn - jump to nnn
    case 0x1000: {
      pc = opcode & 0x0FFF;
      break;
    }

    // call subroutine at 0x0FFF
    // TODO: eror check stack pointer
    case 0x2000: {
      stack[sp++] = pc;
      pc = opcode & 0x0FFF;
      break;
    }

    // 3xkk - skip next instruction if Vx == kk
    case 0x3000: {
      uint8_t reg = (opcode & 0x0F00) >> 8;
      uint8_t val = opcode & 0x00FF;

      if (regs[reg] == val) {
        pc += 2;
      }
      break;
    }

    // 4xkk - skip next instruction if Vx != kk
    case 0x4000: {
      uint8_t reg = (opcode & 0x0F00) >> 8;
      uint8_t val = opcode & 0x00FF;

      if (regs[reg] != val) {
        pc += 2;
      }
      break;
    }

    // 5xy0 - skip instruction if Vx == Vy
    case 0x5000: {
      uint8_t reg1 = (opcode & 0x0F00) >> 8;
      uint8_t reg2 = (opcode & 0x00F0) >> 4;

      if (regs[reg1] == regs[reg2]) {
        pc += 2;
      }
      break;
    }

    // 6xkk - Vx = kk 
    case 0x6000: {
      uint8_t reg = (opcode & 0x0F00) >> 8;
      regs[reg] = opcode & 0x00FF;
      break;
    }

    // 7xkk - Vx = Vx + kk
    case 0x7000: {
      regs[(opcode & 0x0F00) >> 8] += opcode & 0x00FF;
      break;
    }

    case 0x8000: {
      switch (opcode & 0x000F) {
        // 8xy0 - Vx |= Vy
        case 0x0000: {
          uint8_t reg1 = (opcode & 0x0F00) >> 8;
          uint8_t reg2 = (opcode & 0x00F0) >> 4;
          regs[reg1] = regs[reg2];
          break;
        }

        // 8xy1 - Vx |= Vy
        case 0x0001: {
          uint8_t reg1 = (opcode & 0x0F00) >> 8;
          uint8_t reg2 = (opcode & 0x00F0) >> 4;
          regs[reg1] |= regs[reg2];
          break;
        }

        // 8xy2 - Vx &= Vy
        case 0x0002: {
          uint8_t reg1 = (opcode & 0x0F00) >> 8;
          uint8_t reg2 = (opcode & 0x00F0) >> 4;
          regs[reg1] &= regs[reg2];
          break;
        }

        // 8xy3 - Vx ^= Vy
        case 0x0003: {
          uint8_t reg1 = (opcode & 0x0F00) >> 8;
          uint8_t reg2 = (opcode & 0x00F0) >> 4;
          regs[reg1] ^= regs[reg2];
          break;
        }

        // 8xy4 - Vx += Vy, VF = carry
        case 0x0004: {
          uint8_t reg1 = (opcode & 0x0F00) >> 8;
          uint8_t reg2 = (opcode & 0x00F0) >> 4;
          regs[0xF] = regs[reg2] > 0xFF - regs[reg1];
          regs[reg1] += regs[reg2];
          break;
        }

        // 8xy5 - Vx -= Vy, VF = NOT carry
        case 0x0005: {
          uint8_t reg1 = (opcode & 0x0F00) >> 8;
          uint8_t reg2 = (opcode & 0x00F0) >> 4;
          regs[0xF] = regs[reg2] <= regs[reg1];
          regs[reg1] -= regs[reg2];
          break;
        }
        
        // 8xy6 - set VF if least sig was 1 and shift Vx right 1
        case 0x0006: {
          uint8_t reg = (opcode & 0x0F00) >> 8;
          regs[0xF] = regs[reg] & 1;
          regs[reg] >>= 1;
          break;
        }

        // 8xy7 - Vx = Vy - Vx, VF = NOT borrow
        case 0x0007: {
          uint8_t reg1 = (opcode & 0x0F00) >> 8;
          uint8_t reg2 = (opcode & 0x00F0) >> 4;
          regs[0xF] = regs[reg2] >= regs[reg1];
          regs[reg1] = regs[reg2] - regs[reg1];
          break;
        }

        // 8xyE - set VF if most sig was 1 and shift Vx left
        case 0x000E: {
          uint8_t reg = (opcode & 0x0F00) >> 8;
          regs[0xF] = regs[reg] >> 7;
          regs[reg] <<= 1;
          break;
        }

        default:
          cout << "RUH ROH RAGGY! Unknown opcode: " << int(opcode) << "\n";
          exit(0);

      } 
      break;
    }

    // 9xy0 - skip instruction if Vx != Vy
    case 0x9000: {
      uint8_t reg1 = (opcode & 0x0F00) >> 8;
      uint8_t reg2 = (opcode & 0x00F0) >> 4;

      if (regs[reg1] != regs[reg2]) {
        pc += 2;
      }
      break;
    }
   
    // Annn - I = nnn
    case 0xA000: {
      reg_I = opcode & 0x0FFF;
      break;
    }

    // Bnnn - jump to V0 + nnn
    case 0xB000: {
      pc = (opcode & 0x0FFF) + regs[0];
      break;
    }

    // Cxkk - Vx = rand() & kk
    case 0xC000: {
      uint8_t reg = (opcode & 0x0F00) >> 8;

      // TODO: make randomness (oxymoron? :D)
      regs[reg] = 7 & (opcode & 0x00FF);
      break;
    }

    // TODO: error check bounds
    // Dxyk - use k bytes starting at reg_I to draw sprites on window at (Vx, Vy)
    case 0xD000: {
      uint8_t x_start = regs[(opcode & 0x0F00) >> 8];
      uint8_t y_start = regs[(opcode & 0x00F0) >> 4];
      uint8_t num_bytes = opcode & 0x000F;
      regs[0xF] = 0;

      uint8_t x_coor;
      uint8_t y_coor;
      uint8_t mem_val;

      for (int i = 0; i < num_bytes; i++) {
        for (int j = 0; j < 8; j++) {
          x_coor = (x_start + j) % WINDOW_X_MAX;
          y_coor = (y_start + i) % WINDOW_Y_MAX;
          mem_val = memory[reg_I + i] & (1 << (7 - j));
          
          // set 0xF register if a pixel is being turned off
          if (mem_val && window[x_coor][y_coor]) {
            regs[0xF] = 1;
          }

          // set new pixel value
          window[x_coor][y_coor] ^= bool(mem_val);
        }
      }
      redraw = true;
      break;
    }

    case 0xE000: {
      uint8_t key_val = regs[(opcode & 0x0F00) >> 8];
      switch (opcode & 0x00FF) {
        // skip next instruction if key with value Vx is being pressed
        case 0x009E: {
          if (key_down[key_val]) {
            pc += 2;
          }
          break;
        }
        // skip next instruction if key with value Vx is NOT being pressed
        case 0x00A1: {
          if (!key_down[key_val]) {
            pc += 2;
          }
          break;
        }

        default:
          cout << "RUH ROH RAGGY! Unknown opcode: " << int(opcode) << "\n";
          exit(0);
      }
      break;
    }

    case 0xF000: {
      switch (opcode & 0x00FF) {
        // Fx07 - set Vx = delay timer
        case 0x0007: {
          regs[(opcode & 0x0F00) >> 8] = reg_delay;
          break;
        }

        // Fx0A - set Vx = first key press
        case 0x000A: {
          bool found_key = false;
          for (int i = 0; i < NUM_KEYS; i++) {
            if (key_down[i]) {
              regs[(opcode & 0x0F00) >> 8] = i;
              found_key = true;
              break;
            }
          }

          if (!found_key) {
            pc -= 2;
            return;
          }
          break;
        }

        // Fx15 - set reg_delay = Vx
        case 0x0015: {
          reg_delay = regs[(opcode & 0x0F00) >> 8];
          break;
        }

        // Fx18 - set reg_sound = Vx
        case 0x0018: {
          reg_sound = regs[(opcode & 0x0F00) >> 8]; 
          break;
        }

        // Fx1E - reg_I += Vx
        case 0x001E: {
          regs[0xF] = reg_I + regs[(opcode & 0x0F00) >> 8] > 0xFFF;
          reg_I += regs[(opcode & 0x0F00) >> 8];
          break;
        }

        // Fx29 - reg_I = Vx hex sprite location
        case 0x0029: {
          uint8_t val = regs[(opcode & 0x0F00) >> 8];
          reg_I = val * HEX_SPRITE_SIZE;
          break;
        }

        // Fx33 - reg_I = BCD rep of Vx
        case 0x0033: {
          uint8_t val = regs[(opcode & 0x0F00) >> 8];

          memory[reg_I] = (val / 100) % 10;
          memory[reg_I + 1] = (val / 10) % 10;
          memory[reg_I + 2] = val % 10;
          break;
        }

        // 0xFx55 - write regs V0-Vx into memory starting at I
        case 0x0055: {
          uint8_t num_regs = (opcode & 0x0F00) >> 8;
          memcpy(&memory[reg_I], regs, num_regs);
          break;
        };

        // 0xFx65 - read from memory starting at I into regs V0-Vx 
        case 0x0065: {
          uint8_t num_regs = (opcode & 0x0F00) >> 8;
          memcpy(regs, &memory[reg_I], num_regs);
          break;
        }

        default:
          cout << "RUH ROH RAGGY! Unknown opcode: " << int(opcode) << "\n";
          exit(0);
      }
      break;
    }

    default:
      cout << "RUH ROH RAGGY! Unknown opcode: " << int(opcode) << "\n";
      exit(0);
  }

  if (reg_delay > 0) {
    reg_delay--;
  }
  if (reg_sound > 0) {
    reg_sound--;
  }
  cycles++;
  // cout << int(cycles) << " CYCLE DONE\n";
  // print_window();
}

