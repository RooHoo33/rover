#include "multiboot.h"
#include "stdlib.h"

#define KERNEL_START 0xC0000000
#define PAGE_FLAG_PRESENT (1 << 0)
#define PAGE_FLAG_WRITE (1 << 1)

extern uint32_t kernel_virtual_start;
extern uint32_t kernel_physical_start;
extern uint32_t kernel_virtual_end;

extern uint32_t initial_page_dir[1024];
void init_memory(uint32_t memory_high, uint32_t physical_alloc_start);


void map_virtual_memory_from_frame(uint32_t virtual_memory_addr,
                                   uint32_t frame_physical_addr);
uint32_t get_unused_frame();
