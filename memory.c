#include "memory.h"
#include "klogger.h"
#include "stdlib.h"

static uint32_t page_frame_max;
static uint32_t page_frame_min;
static uint32_t page_frame_count;

#define NUM_PAGES_DIRS 256
#define NUM_PAGE_FRAMES (0x100000000 / 0x1000)
uint32_t page_directories[NUM_PAGES_DIRS][1024] __attribute__((aligned(4096)));
uint8_t page_directories_used[NUM_PAGES_DIRS];

uint8_t physical_memory_bitmap[NUM_PAGE_FRAMES / 8];

uint32_t get_unused_frame() {
  for (uint32_t i = 0; i < NUM_PAGE_FRAMES / 8; i++) {
    uint8_t page_frame_bitmap = physical_memory_bitmap[i];
    for (uint32_t j = 0; j < 8; j++) {
      if ((page_frame_bitmap & (1U << j)) == 0) {
        (physical_memory_bitmap[i] |= (1U << ((j) % 8)));
        int32_t frame_physical_location = (page_frame_min + (i * 8) + j) * 4096;
        return frame_physical_location;
      }
    }
  }
  return 0;
}

void map_kernel_code() {
  uint32_t virt = (uint32_t)&kernel_virtual_start;
  uint32_t phys = (uint32_t)&kernel_physical_start;
  uint32_t end_virt = (uint32_t)&kernel_virtual_end;

  // align on 4k memory address
  end_virt = (end_virt + 0xFFF) & ~0xFFF;

  while (virt < end_virt) {
    map_virtual_memory_from_frame(virt, phys);

    // Move to the next 4KB page
    virt += 4096;
    phys += 4096;
  }
}

static inline void flush_tlb() {
  uint32_t cr3;
  asm volatile("mov %%cr3, %0" : "=r"(cr3));
  asm volatile("mov %0, %%cr3" ::"r"(cr3));
}

void invalidate(uint32_t virtual_addresss) {
  asm volatile("invlpg %0" ::"m"(virtual_addresss));
}
void map_virtual_memory_from_frame(uint32_t virtual_memory_addr,
                                   uint32_t frame_physical_addr) {
  uint32_t page_directory_index = virtual_memory_addr >> 22;
  uint32_t page_table_index = (virtual_memory_addr >> 12) & 0x03FF;

  if ((initial_page_dir[page_directory_index] & PAGE_FLAG_PRESENT) == 0) {
    // the page entry doesnt exist
    uint32_t new_table_physical = get_unused_frame();

    initial_page_dir[page_directory_index] =
        new_table_physical | PAGE_FLAG_PRESENT | PAGE_FLAG_WRITE;
    uint32_t *new_table_virtual =
        (uint32_t *)(0xFFC00000 + (page_directory_index * 0x1000));
    memset(new_table_virtual, 0, 4096);
  }

  // invalidate(virtual_memory_addr);
  uint32_t *page_table =
      (uint32_t *)(0xFFC00000 + (page_directory_index * 0x1000));
  page_table[page_table_index] =
      frame_physical_addr | PAGE_FLAG_PRESENT | PAGE_FLAG_WRITE;
}

void init_physical_memory(uint32_t memory_high, uint32_t physical_alloc_start) {
  page_frame_min = (physical_alloc_start + 4096 - 1) / 4096;
  page_frame_max = memory_high / 4096;
  page_frame_count = 0;

  memset(physical_memory_bitmap, 0, sizeof(physical_memory_bitmap));
}

void init_memory(uint32_t memory_high, uint32_t physical_alloc_start) {
  initial_page_dir[0] = 0;
  invalidate(0);
  initial_page_dir[1023] = ((uint32_t)initial_page_dir - KERNEL_START) |
                           PAGE_FLAG_PRESENT | PAGE_FLAG_WRITE;
  invalidate(0xFFFFF000);
  init_physical_memory(memory_high, physical_alloc_start);
  memset(page_directories, 0, sizeof(page_directories));
  memset(page_directories_used, 0, NUM_PAGES_DIRS);

  map_kernel_code();
  flush_tlb();
  // uint32_t *a = (uint32_t *)0x00400000;
  //*a = 100;
}
