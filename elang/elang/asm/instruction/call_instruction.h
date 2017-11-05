#pragma once

#ifndef ELANG_CALL_INSTRUCTION_H
#define ELANG_CALL_INSTRUCTION_H

#include "../../vm/machine.h"

#include "instruction_base.h"

namespace elang::easm::instruction{
	class call : public base{
	public:
		template <typename... args_types>
		explicit call(args_types &&... args)
			: base("call", std::forward<args_types>(args)...){}

		virtual value_type_id_type required_value_type() const override{
			return value_type_id_type::qword;
		}

		virtual void validate_operands() const override{
			if (operands_.size() != 1u)
				throw error_type::bad_operand_count;

			if (operands_[0]->id() != operand_id_type::label)
				throw error_type::bad_operand;
		}

		virtual void execute_and_update_instruction_pointer(register_type &instruction_pointer) const override{
			execute();
		}

		virtual void execute() const override{
			elang::vm::machine::cached_registers.instruction_pointer->inc(instruction_bytes());
			elang::vm::machine::cached_registers.instruction_pointer->push_onto_stack();
			elang::vm::machine::cached_registers.instruction_pointer->write(operands_[0]->read_64bits());
		}
	};
}

#endif /* !ELANG_CALL_INSTRUCTION_H */
