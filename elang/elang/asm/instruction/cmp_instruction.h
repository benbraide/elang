#pragma once

#ifndef ELANG_CMP_INSTRUCTION_H
#define ELANG_CMP_INSTRUCTION_H

#include "../../vm/machine.h"

#include "instruction_base.h"

namespace elang::easm::instruction{
	class cmp : public base{
	public:
		template <typename... args_types>
		explicit cmp(args_types &&... args)
			: base("cmp", std::forward<args_types>(args)...){}

		virtual void apply_required_value_type() override{
			if (operands_.size() == 2u){//Make both operands typed
				operands_[1]->apply_value_type(operands_[0]->value_type());
				operands_[0]->apply_value_type(operands_[1]->value_type());
			}
		}

		virtual void validate_operands() const override{
			if (operands_.size() != operand_count_())
				throw error_type::bad_operand_count;

			if (operands_[0]->id() != operand_id_type::register_value)
				throw error_type::bad_operand;

			check_mismatch_();
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

			throw error_type::ambiguous_operation;
		}

	protected:
		virtual size_type operand_count_() const{
			return 2u;
		}

		virtual void check_mismatch_() const{
			if (operands_[0]->value_type() != operands_[1]->value_type())
				throw error_type::operands_type_mismatch;

			if (operands_[0]->value_type() == value_type_id_type::unknown)
				throw error_type::ambiguous_operation;
		}

		template <typename target_type>
		void execute_() const{
			auto reg = register_target_();
			if (reg == nullptr)//Use operand
				target_()->write(static_cast<__int64>(left_()->read<target_type>() - right_()->read<target_type>()));
			else//Use register
				reg->write(static_cast<__int64>(left_()->read<target_type>() - right_()->read<target_type>()));
		}

		virtual operand_ptr_type target_() const{
			return operands_[0];
		}

		virtual register_type *register_target_() const{
			return elang::vm::machine::cached_registers.compare_target;
		}

		virtual operand_ptr_type left_() const{
			return operands_[0];
		}

		virtual operand_ptr_type right_() const{
			return operands_[1];
		}
	};
}

#endif /* !ELANG_CMP_INSTRUCTION_H */
