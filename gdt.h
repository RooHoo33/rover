
#include "stdlib.h"

struct gdt_entry {
    uint16_t limit;
    uint16_t base_low;
    uint8_t base_middle;
    uint8_t access;
    uint8_t flags;
    uint8_t base_high;
}__attribute__((packed));


struct gdt_pointer{
    uint16_t limit;
    unsigned int base;
}__attribute__((packed));

void initGdt();
void setGdtGate(uint32_t number, uint32_t, uint32_t limit, uint8_t access, uint8_t granularity);
