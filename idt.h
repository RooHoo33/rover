

#ifndef IDT
#define IDT
#include "stdlib.h"
struct InterruptDescriptor32 {
  uint16_t offset_low;     // offset bits 0..15
  uint16_t selector;       // a code segment selector in GDT or LDT
  uint8_t zero;            // unused, set to 0
  uint8_t type_attributes; // gate type, dpl, and p fields
  uint16_t offset_high;    // offset bits 16..31
} __attribute__((packed));

struct InteruptDescriptorTablePointer {
  unsigned short limit;
  uintptr_t base;
} __attribute__((packed));

void initIdt();
extern void idt_load(uintptr_t);
void idt_set_gate(unsigned char num, unsigned long base, unsigned short sel,
                  unsigned char flags);
#endif
