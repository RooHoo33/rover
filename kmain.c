
#include "gdt.h"
#include "idt.h"
#include "irq.h"
#include "isr.h"
#include "keyboard.h"
#include "klogger.h"
#include "memory.h"
#include "stdlib.h"

// Define the screen dimensions for standard VGA text mode
// #define VGA_WIDTH 80
// #define VGA_HEIGHT 25

void kmain(uint32_t magic_number, struct multiboot_info *boot_info) {
  printf("Magic number: %x\n", magic_number);
  clear_screen();
  initGdt();
  initIdt();
  isrs_install();
  init_irq();
  // timer_install();
  keyboard_install();
  __asm__ __volatile__("sti"); // re-enable isr inturupts
  //
  //
  //
  uint32_t mod1 = *(uint32_t *)(boot_info->mods_addr + 4);
  uint32_t physical_alloc_start = (mod1 + 0xFFF) & ~0xFFF;
  init_memory(boot_info->mem_upper, physical_alloc_start);
  // unsigned short *vga_buffer = (unsigned short *)0xB8000;

  // unsigned short green_pixel = 0x2000 | ' ';
  __asm__ volatile("int $0x03");
  //__builtin_trap();
  BOCHS_BREAK();
  //puts("\n\nJACK IS SUPER COOL");
  // Loop through every row and column to fill the screen
  // for (int y = 0; y < VGA_HEIGHT; y++) {
  //  for (int x = 0; x < VGA_WIDTH; x++) {
  //    // Calculate the 1D index in the buffer
  //    int index = y * VGA_WIDTH + x;

  //    if (x == 0) {
  //      vga_buffer[index] = 0x1f00 | 'J';
  //    } else if (x == 1) {
  //      vga_buffer[index] = 0x1f00 | 'a';
  //    } else if (x == 2) {
  //      vga_buffer[index] = 0x1f00 | 'C';
  //    } else if (x == 3) {
  //      vga_buffer[index] = 0x1f00 | 'K';
  //    } else {
  //      vga_buffer[index] = green_pixel;
  //    }
  //    // Write the pixel to video memory
  //  }
  //}
  //

  // int a = 11 / 0;
  //  Infinite loop to keep the kernel alive
  while (1)
    ;
}
