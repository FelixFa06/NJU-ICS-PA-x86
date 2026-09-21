#include "cpu/instr.h"
/*
Put the implementations of `push' instructions here.
*/
static void instr_execute_1op()
{
    operand_read(&opr_src);
    if (opr_src.data_size < data_size)
    {
        opr_src.val = sign_ext(opr_src.val, opr_src.data_size);
        if (data_size == 16)
            opr_src.val = opr_src.val & 0xffff;
    }
    cpu.esp = cpu.esp - data_size / 8;
    vaddr_write(cpu.esp, SREG_SS, data_size / 8, opr_src.val);
}

make_instr_impl_1op(push, r, v)
make_instr_impl_1op(push, i, v)
make_instr_impl_1op(push, i, b)
make_instr_impl_1op(push, rm, v)