#include "cpu/instr.h"
/*
Put the implementations of `ret' instructions here.
*/
make_instr_func(ret_near)
{
    cpu.eip = vaddr_read(cpu.esp, SREG_SS, 4);
    cpu.esp += 4;

    print_asm_0("ret", "", 1);

    return 0;
}

make_instr_func(ret_near_imm16)
{
    OPERAND imm16;
    imm16.type = OPR_IMM;
    imm16.sreg = SREG_CS;
    imm16.data_size = 16;
    imm16.addr = eip + 1;
    operand_read(&imm16);

    cpu.eip = vaddr_read(cpu.esp, SREG_SS, 4);
    cpu.esp += 4 + imm16.val;

    print_asm_1("ret", "", 1 + 4, &imm16);

    return 0;
}