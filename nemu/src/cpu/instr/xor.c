#include "cpu/instr.h"
/*
Put the implementations of `xor' instructions here.
*/
static void instr_execute_2op()
{
    opperand_read(&opr_src);
    opperand_read(&opr_dest);
    opr_dest.val = alu_xor(opr_src.val, opr_dest.val, opr_dest.data_size);
    opperand_write(&opr_dest);
}

make_instr_impl_2op(xor, r, rm, b)
make_instr_impl_2op(xor, r, rm, v)
make_instr_impl_2op(xor, rm, r, b)
make_instr_impl_2op(xor, rm, r, v)
make_instr_impl_2op(xor, i, a, b)
make_instr_impl_2op(xor, i, a, v)
make_instr_impl_2op(xor, i, rm, b)
make_instr_impl_2op(xor, i, rm, v)
make_instr_impl_2op(xor, i, rm, bv)