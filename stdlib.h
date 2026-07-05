#ifndef STDINT_I386_H
#define STDINT_I386_H

/* Unsigned exact-width types */
typedef unsigned char uint8_t;       /* 8-bit */
typedef unsigned short uint16_t;     /* 16-bit */
typedef unsigned int uint32_t;       /* 32-bit */
typedef unsigned long long uint64_t; /* 64-bit */

/* Signed exact-width types */
typedef signed char int8_t;       /* 8-bit */
typedef signed short int16_t;     /* 16-bit */
typedef signed int int32_t;       /* 32-bit */
typedef signed long long int64_t; /* 64-bit */

/* Pointer-sized integers (32-bit on i386) */
typedef unsigned int uintptr_t;
typedef signed int intptr_t;

/* Optional: Common standard definitions for bare-metal */
typedef unsigned int size_t;
typedef signed int ssize_t;

#endif /* STDINT_I386_H */

#define BOCHS_BREAK() __asm__ __volatile__("xchg %bx, %bx")

void puts(char *);
void clear_screen();

unsigned char inportb(unsigned short _port);
void outportb(unsigned short _port, unsigned char _data);

void memcpy(void *dest, void *src, size_t n);
void memset(void *ptr, int value, size_t num);
