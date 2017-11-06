#pragma once

#ifndef ELANG_EXTENDED_ARITHMETIC_INSTRUCTION_H
#define ELANG_EXTENDED_ARITHMETIC_INSTRUCTION_H

#include "../arithmetic_instruction.h"

namespace elang::easm::instruction{
	template <instruction_operator_id id>
	class extended_binary_arithmetic : public binary_arithmetic<id>{
	public:
		typedef binary_arithmetic<id> base_type;

		template <typename... args_types>
		explicit extended_binary_arithmetic(args_types &&... args)
			: base_type(std::forward<args_types>(args)...){
			name_ = ("%" + name_);
		}

	protected:
		virtual size_type operand_count_() const override{
			return 3u;
		}

		virtual void check_mismatch_() const{
			base_type::check_mismatch_();
			if (operands_[0]->value_type() != operands_[2]->value_type())
				throw error_type::operands_type_mismatch;
		}

		virtual operand_ptr_type left_() const override{
			return operands_[1];
		}

		virtual operand_ptr_type right_() const override{
			return operands_[2];
		}
	};

	using ex_add	= extended_binary_arithmetic<instruction_operator_id::add>;
	using ex_sub	= extended_binary_arithmetic<instruction_operator_id::sub>;

	using ex_mult	= extended_binary_arithmetic<instruction_operator_id::mult>;
	using ex_div	= extended_binary_arithmetic<instruction_operator_id::div>;
	using ex_mod	= extended_binary_arithmetic<instruction_operator_id::mod>;

	using ex_and_	= extended_binary_arithmetic<instruction_operator_id::and_>;
	using ex_xor_	= extended_binary_arithmetic<instruction_operator_id::xor_>;
	using ex_or_	= extended_binary_arithmetic<instruction_operator_id::or_>;

	using ex_sal	= extended_binary_arithmetic<instruction_operator_id::sal>;
	using ex_sar	= extended_binary_arithmetic<instruction_operator_id::sar>;
}

#endif /* !ELANG_EXTENDED_ARITHMETIC_INSTRUCTION_H */
