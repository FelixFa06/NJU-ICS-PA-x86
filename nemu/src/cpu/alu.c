#include "cpu/cpu.h"

void set_PF(uint32_t result)
{
	cpu.eflags.PF = 1;
	for (int i = 0; i < 8; i++, result >>= 1)
		cpu.eflags.PF ^= result & 1;
}

void set_ZF(uint32_t result, size_t data_size)
{
	cpu.eflags.ZF = !(result & (0xffffffff >> (32 - data_size)));
}

void set_SF(uint32_t result, size_t data_size)
{
	result = sign_ext(result & (0xffffffff >> (32 - data_size)), data_size);
	cpu.eflags.SF = sign(result);
}

void set_CF_add(uint32_t result, uint32_t src, uint32_t cf, size_t data_size)
{
	result = sign_ext(result & (0xffffffff >> (32 - data_size)), data_size);
	src = sign_ext(src & (0xffffffff >> (32 - data_size)), data_size);
	cpu.eflags.CF = result < src || (result == src && cf);
}

void set_CF_sub(uint32_t dest, uint32_t src, uint32_t cf, size_t data_size)
{
	dest = sign_ext(dest & (0xffffffff >> (32 - data_size)), data_size);
	src = sign_ext(src & (0xffffffff >> (32 - data_size)), data_size);
	cpu.eflags.CF = dest < src || (dest == src && cf);
}

void set_CF_sal(uint32_t src, uint32_t dest, size_t data_size)
{
	dest = sign_ext(dest & (0xffffffff >> (32 - data_size)), data_size);
	cpu.eflags.CF = src > data_size ? 0 : (dest >> (data_size - src) & 1);
}

void set_CF_sar(uint32_t src, uint32_t dest, size_t data_size)
{
	dest = sign_ext(dest & (0xffffffff >> (32 - data_size)), data_size);
	cpu.eflags.CF = src > data_size ? sign(dest) : (dest >> (src - 1) & 1);
}

void set_CF_shr(uint32_t src, uint32_t dest, size_t data_size)
{
	dest = dest & (0xffffffff >> (32 - data_size));
	cpu.eflags.CF = src > data_size ? 0 : (dest >> (src - 1) & 1);
}

void set_OF_add(uint32_t result, uint32_t src, uint32_t dest, size_t data_size)
{
	result = sign_ext(result & (0xffffffff >> (32 - data_size)), data_size);
	src = sign_ext(src & (0xffffffff >> (32 - data_size)), data_size);
	dest = sign_ext(dest & (0xffffffff >> (32 - data_size)), data_size);
	cpu.eflags.OF = sign(dest) == sign(src) && sign(result) != sign(dest);
}

void set_OF_sub(uint32_t result, uint32_t src, uint32_t dest, size_t data_size)
{
	result = sign_ext(result & (0xffffffff >> (32 - data_size)), data_size);
	src = sign_ext(src & (0xffffffff >> (32 - data_size)), data_size);
	dest = sign_ext(dest & (0xffffffff >> (32 - data_size)), data_size);
	cpu.eflags.OF = sign(dest) != sign(src) && sign(result) != sign(dest);
}

void set_CFOF_mul(uint64_t result, size_t data_size)
{
	result >>= data_size;
	cpu.eflags.CF = cpu.eflags.OF = !!(result & (0xffffffff >> (32 - data_size)));
}

void set_CFOF_imul(int64_t result, int32_t sign, size_t data_size)
{
	result >>= data_size;
	if (sign == 1)
		result ^= 0xffffffff >> (32 - data_size);
	cpu.eflags.CF = cpu.eflags.OF = !!(result & (0xffffffff >> (32 - data_size)));
}

uint32_t alu_add(uint32_t src, uint32_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_add(src, dest, data_size);
#else
	uint32_t res = dest + src;
	set_PF(res);
	set_ZF(res, data_size);
	set_SF(res, data_size);
	set_CF_add(res, src, (uint32_t)0, data_size);
	set_OF_add(res, src, dest, data_size);
	return res & (0xFFFFFFFF >> (32 - data_size));
#endif
}

uint32_t alu_adc(uint32_t src, uint32_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_adc(src, dest, data_size);
#else
	uint32_t cf = cpu.eflags.CF;
	uint32_t res = dest + src + cf;
	set_PF(res);
	set_ZF(res, data_size);
	set_SF(res, data_size);
	set_CF_add(res, src, cf, data_size);
	set_OF_add(res, src, dest, data_size);
	return res & (0xFFFFFFFF >> (32 - data_size));
#endif
}

uint32_t alu_sub(uint32_t src, uint32_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_sub(src, dest, data_size);
#else
	uint32_t res = dest - src;
	set_PF(res);
	set_ZF(res, data_size);
	set_SF(res, data_size);
	set_CF_sub(dest, src, (uint32_t)0, data_size);
	set_OF_sub(res, src, dest, data_size);
	return res & (0xFFFFFFFF >> (32 - data_size));
#endif
}

uint32_t alu_sbb(uint32_t src, uint32_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_sbb(src, dest, data_size);
#else
	uint32_t cf = cpu.eflags.CF;
	uint32_t res = dest - src - cf;
	set_PF(res);
	set_ZF(res, data_size);
	set_SF(res, data_size);
	set_CF_sub(dest, src, cf, data_size);
	set_OF_sub(res, src, dest, data_size);
	return res & (0xFFFFFFFF >> (32 - data_size));
#endif
}

uint64_t alu_mul(uint32_t src, uint32_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_mul(src, dest, data_size);
#else
	uint64_t src64 = src & (0xffffffff >> (32 - data_size));
	uint64_t dest64 = dest & (0xffffffff >> (32 - data_size));
	uint64_t res = src64 * dest64;
	set_CFOF_mul(res, data_size);
	return res & (0xffffffffffffffff >> (64 - data_size * 2));
#endif
}

int64_t alu_imul(int32_t src, int32_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_imul(src, dest, data_size);
#else
	int64_t src64 = sign_ext_64(src & (0xffffffff >> (32 - data_size)), data_size);
	int64_t dest64 = sign_ext_64(dest & (0xffffffff >> (32 - data_size)), data_size);
	int64_t res = src64 * dest64;
	int32_t sign = sign_64(src64) ^ sign_64(dest64);
	set_CFOF_imul(res, sign, data_size);
	return res;
#endif
}

// need to implement alu_mod before testing
uint32_t alu_div(uint64_t src, uint64_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_div(src, dest, data_size);
#else
	if (src == 0)
	{
		printf("Floating Point Exception"), fflush(stdout);
		assert(0);
	}
	return (uint32_t)(dest / src) & (0xffffffff >> (32 - data_size));
#endif
}

// need to implement alu_imod before testing
int32_t alu_idiv(int64_t src, int64_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_idiv(src, dest, data_size);
#else
	if (src == 0)
	{
		printf("Floating Point Exception"), fflush(stdout);
		assert(0);
	}
	return (int32_t)(dest / src) & (0xffffffff >> (32 - data_size));
#endif
}

uint32_t alu_mod(uint64_t src, uint64_t dest)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_mod(src, dest);
#else
	if (src == 0)
	{
		printf("Floating Point Exception"), fflush(stdout);
		assert(0);
	}
	return (uint32_t)(dest % src);
#endif
}

int32_t alu_imod(int64_t src, int64_t dest)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_imod(src, dest);
#else
	if (src == 0)
	{
		printf("Floating Point Exception"), fflush(stdout);
		assert(0);
	}
	return (int32_t)(dest % src);
#endif
}

uint32_t alu_and(uint32_t src, uint32_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_and(src, dest, data_size);
#else
	uint32_t res = src & dest;
	set_PF(res);
	set_ZF(res, data_size);
	set_SF(res, data_size);
	cpu.eflags.CF = cpu.eflags.OF = 0;
	return res & (0xFFFFFFFF >> (32 - data_size));
#endif
}

uint32_t alu_xor(uint32_t src, uint32_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_xor(src, dest, data_size);
#else
	uint32_t res = src ^ dest;
	set_PF(res);
	set_ZF(res, data_size);
	set_SF(res, data_size);
	cpu.eflags.CF = cpu.eflags.OF = 0;
	return res & (0xFFFFFFFF >> (32 - data_size));
#endif
}

uint32_t alu_or(uint32_t src, uint32_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_or(src, dest, data_size);
#else
	uint32_t res = src | dest;
	set_PF(res);
	set_ZF(res, data_size);
	set_SF(res, data_size);
	cpu.eflags.CF = cpu.eflags.OF = 0;
	return res & (0xFFFFFFFF >> (32 - data_size));
#endif
}

uint32_t alu_shl(uint32_t src, uint32_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_shl(src, dest, data_size);
#else
	dest &= 0xFFFFFFFF >> (32 - data_size);
	if (src == 0)
		return dest;
	uint32_t res = dest << src;
	set_PF(res);
	set_ZF(res, data_size);
	set_SF(res, data_size);
	set_CF_sal(src, dest, data_size);
	return res & (0xFFFFFFFF >> (32 - data_size));
#endif
}

uint32_t alu_shr(uint32_t src, uint32_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_shr(src, dest, data_size);
#else
	dest &= 0xFFFFFFFF >> (32 - data_size);
	if (src == 0)
		return dest;
	uint32_t res = dest >> src;
	set_PF(res);
	set_ZF(res, data_size);
	set_SF(res, data_size);
	set_CF_shr(src, dest, data_size);
	return res & (0xFFFFFFFF >> (32 - data_size));
#endif
}

uint32_t alu_sar(uint32_t src, uint32_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_sar(src, dest, data_size);
#else
	dest = sign_ext(dest & (0xFFFFFFFF >> (32 - data_size)), data_size);
	if (src == 0)
		return dest & (0xFFFFFFFF >> (32 - data_size));
	uint32_t res = (int32_t)dest >> src;
	set_PF(res);
	set_ZF(res, data_size);
	set_SF(res, data_size);
	set_CF_sar(src, dest, data_size);
	return res & (0xFFFFFFFF >> (32 - data_size));
#endif
}

uint32_t alu_sal(uint32_t src, uint32_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_sal(src, dest, data_size);
#else
	dest &= 0xFFFFFFFF >> (32 - data_size);
	if (src == 0)
		return dest;
	uint32_t res = dest << src;
	set_PF(res);
	set_ZF(res, data_size);
	set_SF(res, data_size);
	set_CF_sal(src, dest, data_size);
	return res & (0xFFFFFFFF >> (32 - data_size));
#endif
}
