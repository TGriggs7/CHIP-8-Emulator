#include "chip8.h"
#include <iostream>

using namespace std;

int main(int argc, char** argv) {
  Chip8 chip8 = Chip8();

  if (argc < 2) {
    cout << "RUH ROH RAGGY! No file attached\n";
  }

  // check input

  // do a bunch of graphical/window set up here (ncurses? SDL?)

  // load the rom into memory
  bool loaded = chip8.load(argv[1]);

  // TODO: track key presses

  // start cycles

    // chip8 cycle

    // take key input (key up/down)

    // reset screen

    // sleep

  while (1) {
    chip8.run_cycle();
  }


  cout << loaded << "\n";
}
