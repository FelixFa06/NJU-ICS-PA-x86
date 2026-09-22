#include "cpu/instr.h"
/*
Put the implementations of `call' instructions here.
*/
make_instr_func(call_near_rel)
{
    vaddr_write(cpu.esp, SREG_SS, 4, cpu.eip & (0xffffffff >> (32 - data_size)));
    cpu.esp -= 4;

    OPERAND rel;
    rel.type = OPR_IMM;
    rel.sreg = SREG_CS;
    rel.data_size = data_size;
    rel.addr = eip + 1;
    operand_read(&rel);

    int offset = sign_ext(rel.val, data_size);
    print_asm_1("call", "", 1 + data_size / 8, &rel);

    cpu.eip += offset;

    return 1 + data_size / 8;
}
make_instr_func(call_near_rm)
{
    vaddr_write(cpu.esp, SREG_SS, 4, cpu.eip & (0xffffffff >> (32 - data_size)));
    cpu.esp -= 4;

    OPERAND rm;
    int len = 1;
    len += modrm_rm(eip + 1, &rm);
    operand_read(&rm);
    print_asm_1("call", "", len, &rm);

    cpu.eip = rm.val & (0xffffffff >> (32 - data_size));

    return 0;
}