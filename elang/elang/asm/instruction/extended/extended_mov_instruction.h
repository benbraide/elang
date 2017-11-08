#pragma once

#ifndef ELANG_EXTENDED_MOV_INSTRUCTION_H
#define ELANG_EXTENDED_MOV_INSTRUCTION_H

#include "../mov_instruction.h"

namespace elang::easm::instruction{
	class extended_mov : public mov{
	public:
		template <typename... args_types>
		explicit extended_mov(args_types &&... args)
			: mov(std::forward<args_types>(args)...){
			name_ = ("%" + name_);
		}

		virtual void validate_operands() const override{
			if (operands_.size() != 2u)
				throw error_type::bad_operand_count;

			if (operands_[0]->id() != operand_id_type::register_value)
				throw error_type::bad_operand;
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
				return execute_float_();
			default:
				break;
			}

			throw error_type::ambiguous_operation;
		}

	protected:
		template <typename value_type>
		void execute_() const{
			operands_[0]->write(static_cast<value_type>(operands_[1]->read_64bits()));
		}

		void execute_float_() const{
			if (operands_[1]->value_type() == value_type_id_type::float_)
				operands_[0]->write(operands_[1]->read<long double>());
			else//Cast
				operands_[0]->write(static_cast<long double>(operands_[1]->read_64bits()));
		}
	};
}

#endif /* !ELANG_EXTENDED_MOV_INSTRUCTION_H */
