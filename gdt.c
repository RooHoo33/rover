
#include "gdt.h"
extern void gdt_flush(struct gdt_pointer *a);
void gdt_flush2(struct gdt_pointer *a) {
  int abc = a->limit;
  abc++;
}

struct gdt_entry gdt_entries[5];
struct gdt_pointer gdt_pointer;

void initGdt() {
  gdt_pointer.limit = (sizeof(struct gdt_entry) * 5) - 1;
  gdt_pointer.base = (unsigned int)&gdt_entries;

  // https://wiki.osdev.org/Global_Descriptor_Table
  setGdtGate(0, 0, 0, 0, 0);                // first null
  setGdtGate(1, 0, 0xFFFFFFFF, 0x9A, 0xCF); // kernel code segment
  setGdtGate(2, 0, 0xFFFFFFFF, 0x92, 0xCF); // kernel data segment
  setGdtGate(3, 0, 0xFFFFFFFF, 0xFA, 0xCF); // user code segment
  setGdtGate(4, 0, 0xFFFFFFFF, 0x9A, 0xCF); // user data segment
  BOCHS_BREAK();
  //  gdt_flush2(&gdt_pointer);
  gdt_flush(&gdt_pointer);
}
void setGdtGate(uint32_t number, uint32_t base, uint32_t limit, uint8_t access,
                uint8_t granularity) {
  gdt_entries[number].base_low = (base & 0xFFFF);
  gdt_entries[number].base_middle = (base >> 16) & 0xFF;
  gdt_entries[number].base_high = (base >> 24) & 0xFF;
  gdt_entries[number].limit = (limit & 0xFFFF);
  gdt_entries[number].flags = (limit >> 16) & 0x0F;
  gdt_entries[number].flags |= (granularity & 0xF0);
  gdt_entries[number].access = access;
}
