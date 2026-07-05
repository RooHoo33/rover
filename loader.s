[BITS 32]
global loader                  
extern kmain

MAGIC_NUMBER equ 0x1BADB002    
FLAGS        equ 0x00000003    ; Bit 0: Align modules on page boundaries
CHECKSUM     equ -(MAGIC_NUMBER + FLAGS)

section .multiboot
align 4
    dd MAGIC_NUMBER
    dd FLAGS
    dd CHECKSUM
    DD 0, 0, 0, 0, 0

    DD 0
    DD 800
    DD 600
    DD 32

; ----------------------------------------------------
; Reserve a real, physical 16KB space in RAM for the Stack
; ----------------------------------------------------
SECTION .bss
ALIGN 16
stack_bottom:
    RESB 16384 * 8
stack_top:


section .boot

global _start
_start:
    MOV ecx, (initial_page_dir - 0xC0000000)
    MOV cr3, ecx

    MOV ecx, cr4
    OR ecx, 0x10
    MOV cr4, ecx

    MOV ecx, cr0
    OR ecx, 0x80000000
    MOV cr0, ecx

    JMP higher_half

section .text
higher_half:
    MOV esp, stack_top
    PUSH ebx
    PUSH eax
    XOR ebp, ebp
    extern kmain
    CALL kmain

halt:
    hlt
    JMP halt


section .data
align 4096
global initial_page_dir
initial_page_dir:
    DD 10000011b
    TIMES 768-1 DD 0

    DD (0 << 22) | 10000011b
    DD (1 << 22) | 10000011b
    DD (2 << 22) | 10000011b
    DD (3 << 22) | 10000011b
    TIMES 256-4 DD 0


;section .text
;align 16
;loader:
;    cli                         ; 1. Turn off interrupts immediately
;
;    ; 2. Initialize Data Segment registers to flat 32-bit space (0x10)
;    mov ax, 0x10                
;    mov ds, ax                  
;    mov es, ax                  
;    mov fs, ax                  
;    mov gs, ax                  
;    mov ss, ax                  
;
;    mov esp, stack_top          
;
;    ; 4. Run your absolute screen check
;    mov word [0x000B8000], 0x4128 
;    jmp .main
;
;.main:
;    sub esp, 8
;    push ebx
;    push eax
;    call kmain 
;    add esp, 8
;    mov eax, 0xCAFEBABE         
;    jmp .dead_lock
;
;; 7. TRAP THE CPU HERE: Stop it from crashing or drifting after C finishes
;.dead_lock:
;    cli
;    hlt
;    jmp .dead_lock
;
;
;global a
;
;a:
;    ret
