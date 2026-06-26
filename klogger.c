

#include "stdlib.h"

#define VGA_WIDTH 80
#define VGA_HEIGHT 25

char screen_buffer[VGA_HEIGHT][VGA_WIDTH];
int current_line = 0;
int current_column = 0;

unsigned short *_vga_buffer = (unsigned short *)0xB8000;
int _get_index(int row, int column) { return row * VGA_WIDTH + column; }

void print_line(int line) {
  for (int i = 0; i < VGA_WIDTH; i++) {
    int index = _get_index(line, i);
    _vga_buffer[index] = 0x0F00 | screen_buffer[line][i];
  }
}
void print_screen() {
  for (int i = 0; i < VGA_HEIGHT; i++) {
    print_line(i);
  }
}
void shift_lines_up() {
  for (int i = 0; i < VGA_HEIGHT - 1; i++) {
    memcpy(screen_buffer[i], screen_buffer[i + 1], VGA_WIDTH);
  }
  for (int i = 0; i < VGA_WIDTH; i++) {
    screen_buffer[VGA_HEIGHT - 1][i] = ' ';
  }
  print_screen();
}

void write_stdout(char *output) {

  while (*output != '\0') {
    if (current_line == VGA_HEIGHT) {
      shift_lines_up();
      current_line--;
      current_column = 0;
    }
    if (current_column == VGA_WIDTH) {
      current_line++;
      current_column = 0;
      return write_stdout(output);
    }
    if (*output == '\n') {
      current_column = 0;
      if (current_line + 1 == VGA_HEIGHT) {
        shift_lines_up();
      } else {
        current_line++;
      }
      output++;
    } else {
      screen_buffer[current_line][current_column] = *output;
      output++;
      current_column++;
    }
  }
  print_screen();
}
