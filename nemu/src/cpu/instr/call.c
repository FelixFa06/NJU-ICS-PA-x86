#include "cpu/instr.h"
/*
Put the implementations of `call' instructions here.
*/
make_instr_func(call_near)
{
    OPERAND rel;
    rel.type = OPR_IMM;
    rel.sreg = SREG_CS;
    rel.data_size = data_size;
    rel.addr = eip + 1;
    operand_read(&rel);

    cpu.esp -= 4;
    uint32_t eip_ret = (cpu.eip + 1 + data_size / 8) & (0xffffffff >> (32 - data_size));
    vaddr_write(cpu.esp, SREG_SS, 4, eip_ret);

    print_asm_1("call", "", 1 + data_size / 8, &rel);

    int offset = sign_ext(rel.val, data_size);
    cpu.eip += offset;

    return 1 + data_size / 8;
}
make_instr_func(call_near_indirect)
{
    OPERAND rm;
    int len = 1;
    rm.data_size = data_size;
    len += modrm_rm(eip + 1, &rm);
    operand_read(&rm);

    cpu.esp -= 4;
    uint32_t eip_ret = (cpu.eip + len) & (0xffffffff >> (32 - data_size));
    vaddr_write(cpu.esp, SREG_SS, 4, eip_ret);

    print_asm_1("call", "", len, &rm);

    cpu.eip = rm.val & (0xffffffff >> (32 - data_size));

    return 0;
}