extern idt 
extern isr1_handler
global isr1
global loadIdt

idtDesc:
    dw 2048
    dd idt

isr1:
    pushad
    call isr1_handler
    popad
    iretd

loadIdt:
    lidt[idtDesc]
    sti
    ret  