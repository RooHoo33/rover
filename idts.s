global idt_load

idt_load:
    MOV eax, [esp+4]
    lidt [eax]
    ret
