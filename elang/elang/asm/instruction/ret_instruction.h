#pragma once

#ifndef ELANG_RET_INSTRUCTION_H
#define ELANG_RET_INSTRUCTION_H

#include "../../vm/machine.h"

#include "instruction_base.h"

namespace elang::easm::instruction{
	class ret : public base{
	public:
		template <typename... args_types>
		explicit ret(args_types &&... args)
			: base("ret", std::forward<args_types>(args)...){}

		virtual void validate_operands() const override{
			if (!operands_.empty())
				throw error_type::bad_operand_count;
		}

		virtual void execute_and_update_instruction_pointer(register_type &instruction_pointer) const override{
			execute();
		}

		virtual void execute() const override{
			elang::vm::machine::cached_registers.instruction_pointer->pop_from_stack();
		}
	};
}

#endif /* !ELANG_RET_INSTRUCTION_H */
