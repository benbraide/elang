#pragma once

#ifndef ELANG_EXTENDED_CMP_INSTRUCTION_H
#define ELANG_EXTENDED_CMP_INSTRUCTION_H

#include "../cmp_instruction.h"

namespace elang::easm::instruction{
	class extended_cmp : public cmp{
	public:
		template <typename... args_types>
		explicit extended_cmp(args_types &&... args)
			: cmp(std::forward<args_types>(args)...){
			name_ = ("%" + name_);
		}

	protected:
		virtual size_type operand_count_() const override{
			return 3u;
		}

		virtual void check_mismatch_() const{
			cmp::check_mismatch_();
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
}

#endif /* !ELANG_EXTENDED_CMP_INSTRUCTION_H */
