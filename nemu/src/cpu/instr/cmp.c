#include "cpu/instr.h"
/*
Put the implementations of `cmp' instructions here.
*/
static void instr_execute_2op()
{
    opperand_read(&opr_src);
    opperand_read(&opr_dest);
    alu_sub(opr_src.val, opr_dest.val, opr_dest.data_size);
}

make_instr_impl_2op(cmp, r, rm, b);
make_instr_impl_2op(cmp, r, rm, v);
make_instr_impl_2op(cmp, rm, r, b);
make_instr_impl_2op(cmp, rm, r, v);
make_instr_impl_2op(cmp, i, a, b);
make_instr_impl_2op(cmp, i, a, v);
make_instr_impl_2op(cmp, i, rm, b);
make_instr_impl_2op(cmp, i, rm, v);
make_instr_impl_2op(cmp, i, rm, bv);