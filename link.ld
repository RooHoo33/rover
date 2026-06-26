ENTRY(loader)

SECTIONS {
    /* Start the kernel execution space at the 1MB physical RAM mark */
    . = 0x00100000;

    .text ALIGN (0x1000) :   
    {
        /* CRITICAL: The Multiboot signature MUST be within the first 8KB */
        *(.multiboot)        
        *(.text)             
    }

    .rodata ALIGN (0x1000) : 
    {
        *(.rodata*)          
    }

    .data ALIGN (0x1000) :   
    {
        *(.data)             
    }

    .bss ALIGN (0x1000) :    
    {
        *(COMMON)            
        *(.bss)              
    }
}
