#include "kmalloc.h"
#include "klogger.h"
#include "memory.h"

#define INIT_MALLO_ENTRY_SIZE 4096
#define KERNEL_HEAP_START 0xD0000000

typedef struct MallocEntry {
  uint32_t size;
  uint8_t is_free;
  struct MallocEntry *next_block;
} __attribute__((packed)) MallocEntry;

static MallocEntry *root_entry;

void init_malloc_entry(uint32_t address) {
  uint32_t free_frame = get_unused_frame();
  map_virtual_memory_from_frame(address, free_frame);
  //BOCHS_BREAK();
  MallocEntry *new_entry = (MallocEntry *)address;
  new_entry->size = INIT_MALLO_ENTRY_SIZE - sizeof(MallocEntry);
  new_entry->is_free = 1;
  new_entry->next_block = NULL;
}

void init_kmalloc() {
  init_malloc_entry(KERNEL_HEAP_START);
  root_entry = (MallocEntry *)KERNEL_HEAP_START;
}

void *kmalloc(uint32_t size) {
  MallocEntry *current_entry = root_entry;
  int i = 0;
  while (1) {
    i++;
    if (i > 4) {
      return NULL;
    }
    if (current_entry != NULL && current_entry->is_free == 1 &&
        current_entry->size >= size) {
      if (current_entry->size == size) {
        current_entry->is_free = 0;
        return (void *)((uint8_t *)current_entry + sizeof(MallocEntry));
      } else if (current_entry->size >= size + sizeof(MallocEntry) + 1) {
        uint32_t new_entry_size =
            current_entry->size - size - sizeof(MallocEntry);
        uint32_t next_memory_address =
            (uint32_t)current_entry + sizeof(MallocEntry) + size;
        MallocEntry *next = (MallocEntry *)next_memory_address;
        next->next_block = current_entry->next_block;
        next->size = new_entry_size;
        next->is_free = 1;
        current_entry->size = size;
        current_entry->is_free = 0;
        current_entry->next_block = next;
        return (void *)((uint8_t *)current_entry + sizeof(MallocEntry));
      } else {
        current_entry->is_free = 0;
        return (void *)((uint8_t *)current_entry + sizeof(MallocEntry));
      }
    } else if (current_entry->next_block == NULL) {
      uint32_t next_memory_address_raw =
          (uint32_t)current_entry + sizeof(MallocEntry) + current_entry->size;

      // just in case we dont land on the 4096 byte page entry
      // edge. Force it to the next mutiple of 4096
      uint32_t next_memory_address = (next_memory_address_raw + 4095) & ~4095;


      if (current_entry->is_free == 1) {
        // frame is free but not big enough so just expand it
        uint32_t free_frame = get_unused_frame();
        map_virtual_memory_from_frame(next_memory_address, free_frame);
        current_entry->size += 4096;
      } else {
        init_malloc_entry(next_memory_address);
        current_entry->next_block = (MallocEntry *)next_memory_address;
        current_entry = (MallocEntry *)next_memory_address;
      }
    } else {
      current_entry = current_entry->next_block;
    }
  }
}
void merge_freed_entries(MallocEntry *entry) {
  if (entry == NULL || entry->is_free == 0) {
    // oops not free entry
    return;
  }
  MallocEntry *current = entry;

  while (current != NULL && current->next_block != NULL &&
         current->next_block->is_free == 1) {
    current->size += sizeof(MallocEntry) + current->next_block->size;
    MallocEntry *old_block = current->next_block;
    uint32_t old_block_size = current->next_block->size + sizeof(MallocEntry);
    current->next_block = current->next_block->next_block;
    memset(old_block, 0, old_block_size);
  }
  return;
}
void kfree(void *ptr) {
  if (ptr == NULL)
    return;

  MallocEntry *previous = root_entry;
  MallocEntry *current = root_entry;

  MallocEntry *entry_to_free =
      (MallocEntry *)((uint8_t *)ptr - sizeof(MallocEntry));
  while (current != NULL) {
    if (entry_to_free == current) {
      current->is_free = 1;
      memset(ptr + sizeof(MallocEntry), 0, current->size);
      merge_freed_entries(previous);
      return;
    }

    previous = current;
    current = current->next_block;
  }
  return;
}
