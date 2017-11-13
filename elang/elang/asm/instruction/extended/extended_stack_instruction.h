#pragma once

#ifndef ELANG_EXTENDED_STACK_INSTRUCTION_H
#define ELANG_EXTENDED_STACK_INSTRUCTION_H

#include "../instruction_base.h"

namespace elang::easm::instruction{
	template <bool push>
	class extended_stack : public base{
	public:
		template <typename... args_types>
		explicit extended_stack(args_types &&... args)
			: base((push ? "%push" : "%pop"), std::forward<args_types>(args)...){}

		virtual void validate_operands() const override{
			if (operands_.empty())
				throw error_type::bad_operand_count;

			for (auto operands : operands_){
				if (operands->id() != operand_id_type::register_value && operands->id() != operand_id_type::memory)
					throw error_type::bad_operand;
			}
		}

		virtual void execute() const override{
			for (auto operands : operands_){
				if (push)
					operands->push_onto_stack();
				else//Pop
					operands->pop_from_stack();
			}
		}
	};

	using extended_pop = extended_stack<false>;
	using extended_push = extended_stack<true>;
}

#endif /* !ELANG_EXTENDED_STACK_INSTRUCTION_H */
