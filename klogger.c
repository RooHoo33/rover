

#include "stdlib.h"

#define VGA_WIDTH 80
#define VGA_HEIGHT 25
#include "stdarg.h"

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
int uint_to_string(uint32_t value, char *output) {
  if (value == 0) {
    output[0] = '0';
    return 1;
  }
  char reverse_buffer[32] = {0};
  int i = 0;
  while (value > 0) {
    reverse_buffer[i++] = (value % 10) + '0';
    value /= 10;
  }
  for (int j = 0; j < i; j++) {
    output[j] = reverse_buffer[i - j - 1];
  }
  return i;
}

int string_to_string(char *value, char *output) {
  int i = 0;
  while (*value != '\0') {
    output[i] = *value;
    value++;
    i++;
  }
  return i;
}

int hex_to_string(uint32_t value, char *output) {
  const char hex_chars[] = "0123456789ABCDEF";

  output[0] = '0';
  output[1] = 'x';
  for (int i = 9; i >= 2; i--) {
    int nibble = value & 0x0F;

    output[i] = hex_chars[nibble];

    value >>= 4;
  }
  return 10;
}

void printf(char *template, ...) {
  va_list args;
  va_start(args, template);
#define output_size 256
  char output_buffer[output_size] = {0};
  char *output = (char *)&output_buffer;
  while (*template != '\0') {
    if (*template == '%') {
      template++;
      if (*template == 'd') {
        int text_writtern = uint_to_string(va_arg(args, uint32_t), output);
        output += text_writtern;
        template++;
      } else if (*template == 's') {
        int text_writtern = string_to_string(va_arg(args, char *), output);
        output += text_writtern;
        template++;
      } else if (*template == 'x') {
        int text_writtern = hex_to_string(va_arg(args, uint32_t), output);
        output += text_writtern;
        template++;
      } else if (*template == '%') {
        *output = '%';
        output++;
        template++;
      }
    } else {
      *output = *template;
      template++;
      output++;
    }
  }
  write_stdout(output_buffer);
}
