#include "cpu/instr.h"
/*
Put the implementations of `leave' instructions here.
*/
make_instr_func(leave)
{
    cpu.esp = cpu.ebp;
    cpu.ebp = vaddr_read(cpu.esp, SREG_SS, 4);
    cpu.esp += 4;

    print_asm_0("leave", "", 1);

    return 1;
}