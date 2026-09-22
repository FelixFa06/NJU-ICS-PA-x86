#include "cpu/instr.h"

make_instr_func(jmp_near_rel)
{
        OPERAND rel;
        rel.type = OPR_IMM;
        rel.sreg = SREG_CS;
        rel.data_size = data_size;
        rel.addr = eip + 1;

        operand_read(&rel);

        int offset = sign_ext(rel.val, data_size);
        // thank Ting Xu from CS'17 for finding this bug
        print_asm_1("jmp", "", 1 + data_size / 8, &rel);

        cpu.eip += offset;

        return 1 + data_size / 8;
}

make_instr_func(jmp_short_)
{
        OPERAND rel;
        rel.type = OPR_IMM;
        rel.sreg = SREG_CS;
        rel.data_size = 8;
        rel.addr = eip + 1;

        operand_read(&rel);

        int offset = sign_ext(rel.val, rel.data_size);
        print_asm_1("jmp", "", 1 + rel.data_size / 8, &rel);

        cpu.eip += offset;

        return 1 + rel.data_size / 8;
}

make_instr_func(jmp_near_rm)
{
        OPERAND rm;
        int len = 1;
        rm.data_size = data_size;
        len += modrm_rm(eip + 1, &rm);

        operand_read(&rm);

        print_asm_1("jmp", "", len, &rm);

        cpu.eip = rm.val;

        return 0;
}