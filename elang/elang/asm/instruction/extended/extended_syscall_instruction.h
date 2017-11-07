#pragma once

#ifndef ELANG_EXTENDED_SYSCALL_INSTRUCTION_H
#define ELANG_EXTENDED_SYSCALL_INSTRUCTION_H

#include "../../../vm/machine.h"

#include "../syscall_instruction.h"

namespace elang::easm::instruction{
	class extended_syscall : public syscall{
	public:
		template <typename... args_types>
		explicit extended_syscall(args_types &&... args)
			: syscall(std::forward<args_types>(args)...){
			name_ = ("%" + name_);
		}

		virtual value_type_id_type required_value_type() const override{
			return value_type_id_type::qword;
		}

		virtual void validate_operands() const override{
			if (operands_.empty())
				throw error_type::bad_operand_count;

			for (auto operand : operands_){
				if (operand->value_type() != value_type_id_type::qword)
					throw error_type::bad_operand;//Error
			}
		}

		virtual void execute() const override{
			unsigned int index = 0;
			for (auto operand : operands_){//Load registers
				auto reg = elang::vm::machine::syscall_register_store.get(index++, false);
				if (reg != nullptr)//Write value to register
					reg->write(operand->read_64bits());
				else//Error
					throw elang::vm::machine_error::no_register;
			}

			elang::vm::syscall::entry();
		}
	};
}

#endif /* !ELANG_EXTENDED_SYSCALL_INSTRUCTION_H */
