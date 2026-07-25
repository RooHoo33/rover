#include "stdlib.h"

void init_kmalloc();
void* kmalloc(uint32_t size);
void kfree(void* ptr);
