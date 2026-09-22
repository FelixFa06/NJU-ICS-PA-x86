#include "cpu/instr.h"
/*
Put the implementations of `lea' instructions here.
*/
static void instr_execute_2op() 
{
    opr_dest.val = opr_src.addr & (0xffffffff >> (32 - data_size));
    operand_write(&opr_dest);
}

make_instr_impl_2op(lea, rm, r, v)