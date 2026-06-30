#include "memory.h"
void init_memory(struct multiboot_info *boot_info) {

  if (boot_info->flags == 0) {
    return;
  }
}
