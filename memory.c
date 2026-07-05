#include "memory.h"
#include "stdlib.h"

static uint32_t page_frame_max;
static uint32_t page_frame_min;
static uint32_t page_frame_count;

#define NUM_PAGES_DIRS 256
#define NUM_PAGE_FRAMES (0x100000000 / 0x1000 / 8)
uint32_t page_directories[NUM_PAGES_DIRS][1024] __attribute__((aligned(4096)));
uint8_t page_directories_used[NUM_PAGES_DIRS];

uint8_t physical_memory_bitmap[NUM_PAGE_FRAMES / 8];

void init_physical_memory(uint32_t memory_high, uint32_t physical_alloc_start) {
  page_frame_min = (physical_alloc_start + 4096 - 1) / 4096;
  page_frame_max = memory_high / 4096;
  page_frame_count = 0;

  memset(physical_memory_bitmap, 0, sizeof(physical_memory_bitmap));
}
void invalidate(uint32_t virtual_addresss) {
  asm volatile("invlpg %0" ::"m"(virtual_addresss));
}
void init_memory(uint32_t memory_high, uint32_t physical_alloc_start) {
  initial_page_dir[0] = 0;
  invalidate(0);
  initial_page_dir[1023] = ((uint32_t)initial_page_dir - KERNEL_START) |
                           PAGE_FLAG_PRESENT | PAGE_FLAG_WRITE;
  invalidate(0xFFFFF000);
  init_physical_memory(memory_high, physical_alloc_start);
  memset(page_directories, 0, sizeof(page_directories_used));
  memset(page_directories_used, 0, NUM_PAGES_DIRS);

  // Will cause page fault now that we have paging
  uint32_t *a = (uint32_t *)0x12345678;
  *a = 100;
}

// void init_memory(struct multiboot_info *boot_info) {
//
//   for (uint32_t i = 0; i < boot_info->mmap_length;
//        i += sizeof(struct multiboot_mmap_entry)) {
//     struct multiboot_mmap_entry *mmap_entry =
//         (struct multiboot_mmap_entry *)(boot_info->mmap_addr + i);
//     printf("L Mem %x, U Mem %x, L Size %x, U Size %x, Type %d\n",
//            mmap_entry->addr_low, mmap_entry->addr_high, mmap_entry->len_low,
//            mmap_entry->len_high, mmap_entry->type);
//   }
// }
