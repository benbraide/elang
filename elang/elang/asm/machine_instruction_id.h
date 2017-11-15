#pragma once

#ifndef ELANG_MACHINE_INSTRUCTION_ID_H
#define ELANG_MACHINE_INSTRUCTION_ID_H

#include "instruction_id.h"

namespace elang::easm::instruction{
	enum class machine_id{
		nop,
		pushb,
		pushw,
		pushd,
		pushq,
		pushf,
		popb,
		popw,
		popd,
		popq,
		popf,
		movb,
		movw,
		movd,
		movq,
		movf,
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
		incb,
		incw,
		incd,
		incq,
		decb,
		decw,
		decd,
		decq,
		addb,
		addw,
		addd,
		addq,
		addf,
		subb,
		subw,
		subd,
		subq,
		subq,
		multb,
		multw,
		multd,
		multq,
		multf,
		divb,
		divw,
		divd,
		divq,
		divf,
		modb,
		modw,
		modd,
		modq,
		andb,
		andw,
		andd,
		andq,
		xorb,
		xorw,
		xord,
		xorq,
		orb,
		orw,
		ord,
		orq,
		salb,
		salw,
		sald,
		salq,
		sarb,
		sarw,
		sard,
		sarq,
		testb,
		testw,
		testd,
		testq,
		notb,
		notw,
		notd,
		notq,
		cmpb,
		cmpw,
		cmpd,
		cmpq,
		db,
		dw,
		dd,
		dq,
		df,
	};

	struct machine_id_utils{
		static machine_id convert(id value){
			switch (value){
			case id::nop:
				break;
			case id::ret:
				break;
			case id::push:
				break;
			case id::pop:
				break;
			case id::int_:
				break;
			case id::call:
				break;
			case id::jmp:
				break;
			case id::jz:
				break;
			case id::jnz:
				break;
			case id::je:
				break;
			case id::jne:
				break;
			case id::jle:
				break;
			case id::jl:
				break;
			case id::jnle:
				break;
			case id::jnl:
				break;
			case id::jge:
				break;
			case id::jg:
				break;
			case id::jnge:
				break;
			case id::jng:
				break;
			case id::loop:
				break;
			case id::inc:
				break;
			case id::dec:
				break;
			case id::not:
				break;
			case id::lea:
				break;
			case id::mov:
				break;
			case id::add:
				break;
			case id::sub:
				break;
			case id::mult:
				break;
			case id::div:
				break;
			case id::mod:
				break;
			case id::and_:
				break;
			case id::xor_:
				break;
			case id::or_:
				break;
			case id::sal:
				break;
			case id::sar:
				break;
			case id::test:
				break;
			case id::cmp:
				break;
			case id::db:
				break;
			case id::dw:
				break;
			case id::dd:
				break;
			case id::dq:
				break;
			case id::df:
				break;
			default:
				break;
			}

			return machine_id::nop;
		}
	};
}

#endif /* !ELANG_MACHINE_INSTRUCTION_ID_H */
