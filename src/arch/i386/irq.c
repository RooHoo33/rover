
#include "irq.h"
#include "idt.h"
#include "isr.h"
#include "stdlib.h"

void timer_phase(int hz) {
  int divisor = 1193180 / hz;     /* Calculate our divisor */
  outportb(0x43, 0x36);           /* Set our command byte 0x36 */
  outportb(0x40, divisor & 0xFF); /* Set low byte of divisor */
  outportb(0x40, divisor >> 8);   /* Set high byte of divisor */
}
void *irq_routines[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

void irq_install_handler(int irq, void (*handler)(struct regs *r)) {
  irq_routines[irq] = handler;
}

void irq_uninstall_handler(int irq) { irq_routines[irq] = 0; }

/* Normally, IRQs 0 to 7 are mapped to entries 8 to 15. This
 *  is a problem in protected mode, because IDT entry 8 is a
 *  Double Fault! Without remapping, every time IRQ0 fires,
 *  you get a Double Fault Exception, which is NOT actually
 *  what's happening. We send commands to the Programmable
 *  Interrupt Controller (PICs - also called the 8259's) in
 *  order to make IRQ0 to 15 be remapped to IDT entries 32 to
 *  47 */
void irq_remap(void) {
  outportb(0x20, 0x11);
  outportb(0xA0, 0x11);
  outportb(0x21, 0x20);
  outportb(0xA1, 0x28);
  outportb(0x21, 0x04);
  outportb(0xA1, 0x02);
  outportb(0x21, 0x01);
  outportb(0xA1, 0x01);
  outportb(0x21, 0x0);
  outportb(0xA1, 0x0);
}

/* We first remap the interrupt controllers, and then we install
 *  the appropriate ISRs to the correct entries in the IDT. This
 *  is just like installing the exception handlers */
void init_irq() {
  irq_remap();

  idt_set_gate(32, (unsigned)irq0, 0x08, 0x8E);
  idt_set_gate(33, (unsigned)irq1, 0x08, 0x8E);
  idt_set_gate(34, (unsigned)irq2, 0x08, 0x8E);
  idt_set_gate(35, (unsigned)irq3, 0x08, 0x8E);
  idt_set_gate(36, (unsigned)irq4, 0x08, 0x8E);
  idt_set_gate(37, (unsigned)irq5, 0x08, 0x8E);
  idt_set_gate(38, (unsigned)irq6, 0x08, 0x8E);
  idt_set_gate(39, (unsigned)irq7, 0x08, 0x8E);
  idt_set_gate(40, (unsigned)irq8, 0x08, 0x8E);
  idt_set_gate(41, (unsigned)irq9, 0x08, 0x8E);
  idt_set_gate(42, (unsigned)irq10, 0x08, 0x8E);
  idt_set_gate(43, (unsigned)irq11, 0x08, 0x8E);
  idt_set_gate(44, (unsigned)irq12, 0x08, 0x8E);
  idt_set_gate(45, (unsigned)irq13, 0x08, 0x8E);
  idt_set_gate(46, (unsigned)irq14, 0x08, 0x8E);
  idt_set_gate(47, (unsigned)irq15, 0x08, 0x8E);

  // timer_phase(100);
}

/* Each of the IRQ ISRs point to this function, rather than
 *  the 'fault_handler' in 'isrs.c'. The IRQ Controllers need
 *  to be told when you are done servicing them, so you need
 *  to send them an "End of Interrupt" command (0x20). There
 *  are two 8259 chips: The first exists at 0x20, the second
 *  exists at 0xA0. If the second controller (an IRQ from 8 to
 *  15) gets an interrupt, you need to acknowledge the
 *  interrupt at BOTH controllers, otherwise, you only send
 *  an EOI command to the first controller. If you don't send
 *  an EOI, you won't raise any more IRQs */
void irq_handler(struct regs *r) {
  /* This is a blank function pointer */
  void (*handler)(struct regs *r);

  /* Find out if we have a custom handler to run for this
   *  IRQ, and then finally, run it */
  handler = irq_routines[r->int_no - 32];
  if (handler) {
    handler(r);
  }

  /* If the IDT entry that was invoked was greater than 40
   *  (meaning IRQ8 - 15), then we need to send an EOI to
   *  the slave controller */
  if (r->int_no >= 40) {
    outportb(0xA0, 0x20);
  }

  /* In either case, we need to send an EOI to the master
   *  interrupt controller too */
  outportb(0x20, 0x20);
}

/* This will keep track of how many ticks that the system
 *  has been running for */
int timer_ticks = 0;

/* Handles the timer. In this case, it's very simple: We
 *  increment the 'timer_ticks' variable every time the
 *  timer fires. By default, the timer fires 18.222 times
 *  per second. Why 18.222Hz? Some engineer at IBM must've
 *  been smoking something funky */
void timer_handler(struct regs *r __attribute__((unused))) {
  /* Increment our 'tick count' */
  timer_ticks++;

  /* Every 18 clocks (approximately 1 second), we will
   *  display a message on the screen */
  if (timer_ticks % 18 == 0) {
    puts("One second has passed");
  }
}

/* Sets up the system clock by installing the timer handler
 *  into IRQ0 */
void timer_install() {
  /* Installs 'timer_handler' to IRQ0 */
  irq_install_handler(0, timer_handler);
}
