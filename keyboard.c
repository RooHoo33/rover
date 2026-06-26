
#include "irq.h"
#include "isr.h"
#include "klogger.h"
#include "stdlib.h"

unsigned char scancode_to_ascii[] = {
    0,   27,  '1',  '2',  '3',  '4', '5', '6',  '7', '8', '9', '0',
    '-', '=', '\b', '\t', 'q',  'w', 'e', 'r',  't', 'y', 'u', 'i',
    'o', 'p', '[',  ']',  '\n', 0,   'a', 's',  'd', 'f', 'g', 'h',
    'j', 'k', 'l',  ';',  '\'', '`', 0,   '\\', 'z', 'x', 'c', 'v',
    'b', 'n', 'm',  ',',  '.',  '/', 0,   '*',  0,   ' '};

char *output = "x";
void keyboard_handler(struct regs *r __attribute__((unused))) {
  unsigned char scancode = inportb(0x60);

  if (scancode & 0x80) {
    // key released
  } else {
    unsigned char ascii_char = scancode_to_ascii[scancode];
    // char message[] = "We got:xn";
    // message[7] = ascii_char;
    output[0] = ascii_char;
    write_stdout(output);
  }
}
void keyboard_install() {
  while (inportb(0x64) & 1) {
    inportb(0x60);
  }
  irq_install_handler(1, keyboard_handler);
}
