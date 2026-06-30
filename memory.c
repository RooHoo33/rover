#include "memory.h"
#include "klogger.h"
void init_memory(struct multiboot_info *boot_info) {

  for (uint32_t i = 0; i < boot_info->mmap_length;
       i += sizeof(struct multiboot_mmap_entry)) {
    struct multiboot_mmap_entry *mmap_entry =
        (struct multiboot_mmap_entry *)(boot_info->mmap_addr + i);
    printf("L Mem %x, U Mem %x, L Size %x, U Size %x, Type %d\n",
           mmap_entry->addr_low, mmap_entry->addr_high, mmap_entry->len_low,
           mmap_entry->len_high, mmap_entry->type);
  }
}
