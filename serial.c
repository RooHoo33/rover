#include "stdlib.h"

#define COM1 0x3F8

int init_serial(void) {
  outportb(COM1 + 1, 0x00); // Disable all interrupts
  outportb(COM1 + 3, 0x80); // Enable DLAB (set baud rate divisor)
  outportb(COM1 + 0, 0x01); // Set divisor to 1 (lo byte) 115200 baud
  outportb(COM1 + 1, 0x00); //                  (hi byte)
  outportb(COM1 + 3, 0x03); // 8 bits, no parity, one stop bit
  outportb(COM1 + 2, 0xC7); // Enable FIFO, clear them, 14-byte threshold
  outportb(COM1 + 4, 0x0B); // IRQs enabled, RTS/DSR set

  /* Perform Loopback test to verify the chip is functional */
  outportb(COM1 + 4, 0x1E); // Put chip into loopback mode
  outportb(COM1 + 0, 0xAE); // Send a test byte (0xAE)

  if (inportb(COM1 + 0) != 0xAE) {
    return 1; // Serial port hardware failure
  }

  /* Put port into normal operational mode (disable loopback) */
  outportb(COM1 + 4, 0x0F);
  return 0;
}

/* Check the Line Status Register to see if the transmit buffer is empty */
int is_transmit_empty(void) { return inportb(COM1 + 5) & 0x20; }

void write_serial_char(char c) {
  while (is_transmit_empty() == 0)
    ;
  outportb(COM1, c);
}

void write_serial_str(const char *str) {
  for (int i = 0; str[i] != '\0'; i++) {
    if (str[i] == '\n') {
      write_serial_char('\r');
    }
    write_serial_char(str[i]);
  }
}
