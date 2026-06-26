[BITS 32]
global loader                  
extern kmain

MAGIC_NUMBER equ 0x1BADB002    
FLAGS        equ 0x00000001    ; Bit 0: Align modules on page boundaries
CHECKSUM     equ -(MAGIC_NUMBER + FLAGS)

section .multiboot
align 4
    dd MAGIC_NUMBER
    dd FLAGS
    dd CHECKSUM

section .text
align 16
loader:
    cli                         ; 1. Turn off interrupts immediately

    ; 2. Initialize Data Segment registers to flat 32-bit space (0x10)
    mov ax, 0x10                
    mov ds, ax                  
    mov es, ax                  
    mov fs, ax                  
    mov gs, ax                  
    mov ss, ax                  

    ; 3. CRITICAL: Initialize the Stack Pointer to our safe buffer ceiling
    mov esp, stack_top          

    ; 4. Run your absolute screen check
    mov word [0x000B8000], 0x4128 
    ;int3
    ; 5. Jump to the main routine to call C safely
    jmp .main

.main:
    sub esp, 8
    call kmain                  ; 6. Execution safely hands off to C here
    add esp, 8
    mov eax, 0xCAFEBABE         
    jmp .main

; 7. TRAP THE CPU HERE: Stop it from crashing or drifting after C finishes
.dead_lock:
    cli
    hlt
    jmp .dead_lock


global a

a:
    ret

; ----------------------------------------------------
; Reserve a real, physical 16KB space in RAM for the Stack
; ----------------------------------------------------
section .bss
align 16
stack_bottom:
    resb 16384                  ; Reserve 16 Kilobytes of space
stack_top:                      ; The stack grows downwards, so esp starts at the top
