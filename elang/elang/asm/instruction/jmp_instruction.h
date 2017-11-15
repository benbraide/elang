#pragma once

#ifndef ELANG_JMP_INSTRUCTION_H
#define ELANG_JMP_INSTRUCTION_H

#include "../../vm/machine.h"

#include "instruction_base.h"

namespace elang::easm::instruction{
	class jmp : public base{
	public:
		template <typename... args_types>
		explicit jmp(args_types &&... args)
			: base("jmp", std::forward<args_types>(args)...){}

		virtual value_type_id_type required_value_type() const override{
			return value_type_id_type::qword;
		}

		virtual void validate_operands() const override{
			if (operands_.size() != 1u)
				throw error_type::bad_operand_count;

			switch (operands_[0]->id()){
			case operand_id_type::register_value:
			case operand_id_type::label:
				break;
			default:
				throw error_type::bad_operand;
				break;
			}

			if (operands_[0]->value_type() != value_type_id_type::qword)
				throw error_type::bad_operand;
		}

		virtual void execute_and_update_instruction_pointer(register_type &instruction_pointer) const override{
			execute();
		}

		virtual void execute() const override{
			if (test_())//Passed test -- do jump
				elang::vm::machine::cached_registers.instruction_pointer->write(operands_[0]->read_64bits());
			else//Advance Instruction Pointer
				elang::vm::machine::cached_registers.instruction_pointer->inc<uint64_type>(instruction_bytes());
		}

	protected:
		virtual bool test_() const{
			return true;
		}
	};

	enum class jmp_instruction_id{
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
	};

	enum class jmp_instruction_cmp_target{
		less,
		less_or_equal,
		equal,
		greater_or_equal,
		greater,
	};

	template <jmp_instruction_id id, jmp_instruction_cmp_target cmp_target, bool invert>
	class conditional_jmp : public jmp{
	public:
		template <typename... args_types>
		explicit conditional_jmp(args_types &&... args)
			: jmp(std::forward<args_types>(args)...){
			switch (id){
			case jmp_instruction_id::jz:
				name_ = "jz";
				break;
			case jmp_instruction_id::jnz:
				name_ = "jnz";
				break;
			case jmp_instruction_id::je:
				name_ = "je";
				break;
			case jmp_instruction_id::jne:
				name_ = "jne";
				break;
			case jmp_instruction_id::jl:
				name_ = "jl";
				break;
			case jmp_instruction_id::jnl:
				name_ = "jnl";
				break;
			case jmp_instruction_id::jle:
				name_ = "jle";
				break;
			case jmp_instruction_id::jnle:
				name_ = "jnle";
				break;
			case jmp_instruction_id::jg:
				name_ = "jg";
				break;
			case jmp_instruction_id::jng:
				name_ = "jng";
				break;
			case jmp_instruction_id::jge:
				name_ = "jge";
				break;
			case jmp_instruction_id::jnge:
				name_ = "jnge";
				break;
			default:
				break;
			}
		}

	protected:
		virtual bool test_() const override{
			typedef jmp_instruction_cmp_target jict;

			auto cmp_state = elang::vm::machine::cached_registers.compare_target->read<__int64>();
			if (cmp_state < 0)//Less
				return ((cmp_target == jict::less || cmp_target == jict::less_or_equal) != invert);

			if (cmp_state == 0)//Equal
				return ((cmp_target == jict::less_or_equal || cmp_target == jict::equal || cmp_target == jict::greater_or_equal) != invert);

			return ((cmp_target == jict::greater || cmp_target == jict::greater_or_equal) != invert);
		}
	};

	using jz	= conditional_jmp<jmp_instruction_id::jz, jmp_instruction_cmp_target::equal, false>;
	using jnz	= conditional_jmp<jmp_instruction_id::jnz, jmp_instruction_cmp_target::equal, true>;

	using je	= conditional_jmp<jmp_instruction_id::je, jmp_instruction_cmp_target::equal, false>;
	using jne	= conditional_jmp<jmp_instruction_id::jne, jmp_instruction_cmp_target::equal, true>;

	using jl	= conditional_jmp<jmp_instruction_id::jl, jmp_instruction_cmp_target::less, false>;
	using jnl	= conditional_jmp<jmp_instruction_id::jnl, jmp_instruction_cmp_target::less, true>;

	using jle	= conditional_jmp<jmp_instruction_id::jle, jmp_instruction_cmp_target::less_or_equal, false>;
	using jnle	= conditional_jmp<jmp_instruction_id::jnle, jmp_instruction_cmp_target::less_or_equal, true>;

	using jg	= conditional_jmp<jmp_instruction_id::jg, jmp_instruction_cmp_target::greater, false>;
	using jng	= conditional_jmp<jmp_instruction_id::jng, jmp_instruction_cmp_target::greater, true>;

	using jge	= conditional_jmp<jmp_instruction_id::jge, jmp_instruction_cmp_target::greater_or_equal, false>;
	using jnge	= conditional_jmp<jmp_instruction_id::jnge, jmp_instruction_cmp_target::greater_or_equal, true>;
}

#endif /* !ELANG_JMP_INSTRUCTION_H */
