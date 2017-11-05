#pragma once

#ifndef ELANG_MOV_INSTRUCTION_H
#define ELANG_MOV_INSTRUCTION_H

#include "instruction_base.h"

namespace elang::easm::instruction{
	class mov : public base{
	public:
		template <typename... args_types>
		explicit mov(args_types &&... args)
			: base("mov", std::forward<args_types>(args)...){}

		virtual void apply_required_value_type() override{
			if (operands_.size() == 2u)
				operands_[1]->apply_value_type(operands_[0]->value_type());
		}

		virtual void validate_operands() const override{
			if (operands_.size() != 2u)
				throw error_type::bad_operand_count;

			switch (operands_[0]->id()){
			case operand_id_type::register_value:
			case operand_id_type::memory:
				break;
			default:
				throw error_type::bad_operand;
				break;
			}

			if (operands_[0]->value_type() != operands_[1]->value_type())
				throw error_type::operands_type_mismatch;

			if (operands_[0]->value_type() == value_type_id_type::unknown)
				throw error_type::ambiguous_operation;
		}

		virtual void execute() const override{
			switch (operands_[0]->value_type()){
			case value_type_id_type::byte:
				return execute_<__int8>();
			case value_type_id_type::word:
				return execute_<__int16>();
			case value_type_id_type::dword:
				return execute_<__int32>();
			case value_type_id_type::qword:
				return execute_<__int64>();
			case value_type_id_type::float_:
				return execute_<long double>();
			default:
				break;
			}

			auto source_memory_size = operands_[1]->memory_size();
			if (source_memory_size != 0u && operands_[0]->memory_size() == source_memory_size)
				operands_[0]->write(nullptr, source_memory_size, numeric_type_id_type::int64);

			throw error_type::ambiguous_operation;
		}

	protected:
		template <typename value_type>
		void execute_() const{
			operands_[0]->write(operands_[1]->read<value_type>());
		}
	};
}

#endif /* !ELANG_MOV_INSTRUCTION_H */
