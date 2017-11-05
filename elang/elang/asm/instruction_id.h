#pragma once

#ifndef ELANG_INSTRUCTION_ID_H
#define ELANG_INSTRUCTION_ID_H

namespace elang::easm::instruction{
	enum class id{
		nop,
		push,
		pop,
		mov,
		lea,
		int_,
		call,
		ret,
		jmp,
		jz,
		jnz,
		je,
		jne,
		jl,
		jnl,
		jle,
		jnle,
		jg,
		jng,
		jge,
		jnge,
		loop,
		inc,
		dec,
		add,
		sub,
		mult,
		div,
		mod,
		and_,
		xor_,
		or_,
		sal,
		sar,
		test,
		not,
		cmp,
		db,
		dw,
		dd,
		dq,
		df,
		times,
	};
}

#endif /* !ELANG_INSTRUCTION_ID_H */
