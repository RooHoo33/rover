
#include "stdlib.h"
#define VGA_WIDTH 80
#define VGA_HEIGHT 25

unsigned short *vga_buffer = (unsigned short *)0xB8000;
int get_index(int row, int column) { return row * VGA_WIDTH + column; }

int column = 0;
int row = 0;
void clear_screen() {
  for (int i = 0; i < VGA_WIDTH; i++) {
    for (int j = 0; j < VGA_HEIGHT; j++) {

      int index = get_index(j, i);
      vga_buffer[index] = 0;
    }
  }
  column = 0;
  row = 0;
}
void puts(char *str) {
  // clear_screen();
  while (*str != '\0') {
    if (column > VGA_WIDTH - 1) {
      row++;
      column = 0;
    }

    int index = get_index(row, column);
    vga_buffer[index] = 0x0F00 | *str;
    // Access the current character co *p
    str++; // Move to the next characte
    column++;
  }
  column = 0;
  row++;
  return;
}

/* We will use this later on for reading from the I/O ports to get data
 *  from devices such as the keyboard. We are using what is called
 *  'inline assembly' in these routines to actually do the work */
unsigned char inportb(unsigned short port) {
  unsigned char ret;
  asm volatile("inb %1, %0" : "=a"(ret) : "Nd"(port));
  return ret;
}

/* We will use this to write to I/O ports to send bytes to devices. This
 *  will be used in the next tutorial for changing the textmode cursor
 *  position. Again, we use some inline assembly for the stuff that simply
 *  cannot be done in C */
void outportb(unsigned short port, unsigned char data) {
  asm volatile("outb %0, %1" : : "a"(data), "Nd"(port));
}

void memcpy(void *dest, void *src, size_t n) {
  char *dest_cast = (char *)dest;
  char *src_cast = (char *)src;
  for (unsigned int i = 0; i < n; i++) {
    dest_cast[i] = src_cast[i];
  }
}
