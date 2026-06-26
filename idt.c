
#include "idt.h"

struct InterruptDescriptor32 idt[256];
struct InteruptDescriptorTablePointer idtp;
/* Use this function to set an entry in the IDT. Alot simpler
 *  than twiddling with the GDT ;) */
void idt_set_gate(unsigned char num, unsigned long base, unsigned short sel,
                  unsigned char flags) {

  /* The interrupt routine's base address */
  idt[num].offset_low = (base & 0xFFFF);
  idt[num].offset_high = (base >> 16) & 0xFFFF;

  /* The segment or 'selector' that this IDT entry will use
   *  is set here, along with any access flags */
  idt[num].selector = sel;
  idt[num].zero = 0;
  idt[num].type_attributes = flags;
}

/* Installs the IDT */
void initIdt() {
  /* Sets the special IDT pointer up, just like in 'gdt.c' */
  idtp.limit = (sizeof(struct InterruptDescriptor32) * 256) - 1;
  idtp.base = (uintptr_t)&idt;

  /* Clear out the entire IDT, initializing it to zeros */
  // memset(&idt, 0, sizeof(struct idt_entry) * 256);

  /* Add any new ISRs to the IDT here using idt_set_gate */

  /* Points the processor's internal register to the new IDT */
  idt_load((uintptr_t)&idtp);
}
