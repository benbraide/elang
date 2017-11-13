#pragma once

#ifndef ELANG_STACK_INSTRUCTION_H
#define ELANG_STACK_INSTRUCTION_H

#include "instruction_base.h"

namespace elang::easm::instruction{
	template <bool push>
	class stack : public base{
	public:
		template <typename... args_types>
		explicit stack(args_types &&... args)
			: base((push ? "push" : "pop"), std::forward<args_types>(args)...){}

		virtual void validate_operands() const override{
			if (operands_.size() != 1u)
				throw error_type::bad_operand_count;

			if (operands_[0]->id() != operand_id_type::register_value && operands_[0]->id() != operand_id_type::memory)
				throw error_type::bad_operand;
		}

		virtual void execute() const override{
			if (push)
				operands_[0]->push_onto_stack();
			else//Pop
				operands_[0]->pop_from_stack();
		}
	};

	using pop = stack<false>;
	using push = stack<true>;
}

#endif /* !ELANG_STACK_INSTRUCTION_H */
