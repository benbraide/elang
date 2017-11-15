#pragma once

#ifndef ELANG_DECL_INSTRUCTION_H
#define ELANG_DECL_INSTRUCTION_H

#include "../../vm/machine.h"

#include "instruction_base.h"

namespace elang::easm::instruction{
	template <elang::vm::machine_value_type_id value_type_id>
	class decl : public base{
	public:
		template <typename... args_types>
		explicit decl(args_types &&... args)
			: base("", std::forward<args_types>(args)...){
			switch (value_type_id){
			case value_type_id_type::byte:
				name_ = "db";
				break;
			case value_type_id_type::word:
				name_ = "dw";
				break;
			case value_type_id_type::dword:
				name_ = "dd";
				break;
			case value_type_id_type::qword:
				name_ = "dq";
				break;
			case value_type_id_type::float_:
				name_ = "df";
				break;
			default:
				break;
			}
		}

		virtual value_type_id_type required_value_type() const override{
			return value_type_id;
		}

		virtual void apply_required_value_type() override{
			base::apply_required_value_type();
		}

		virtual void validate_operands() const override{
			if (operands_.empty())
				throw error_type::bad_operand_count;

			for (auto operand : operands_){
				if (!operand->is_constant())//Constant value required
					throw error_type::bad_operand;
			}
		}

		virtual void write_memory(uint64_type &address, char *buffer) const override{
			if (buffer == nullptr)//Allocate buffer
				buffer = elang::vm::machine::memory_manager.allocate(instruction_bytes(), address)->data.get();

			for (auto operand : operands_){//Write operands
				operand->write_to_memory(buffer, address);
				address += operand->instruction_bytes();
				buffer += operand->instruction_bytes();
			}
		}

		virtual void execute_and_update_instruction_pointer(register_type &instruction_pointer) const override{
			execute();
		}

		virtual void execute() const override{
			throw error_type::no_execution;
		}

	protected:
		virtual size_type this_instruction_bytes_() const override{
			return 0;
		}
	};

	using db = decl<elang::vm::machine_value_type_id::byte>;
	using dw = decl<elang::vm::machine_value_type_id::word>;
	using dd = decl<elang::vm::machine_value_type_id::dword>;
	using dq = decl<elang::vm::machine_value_type_id::qword>;
	using df = decl<elang::vm::machine_value_type_id::float_>;
}

#endif /* !ELANG_DECL_INSTRUCTION_H */
