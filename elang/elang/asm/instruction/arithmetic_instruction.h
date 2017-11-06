#pragma once

#ifndef ELANG_ARITHMETIC_INSTRUCTION_H
#define ELANG_ARITHMETIC_INSTRUCTION_H

#include "instruction_base.h"

namespace elang::easm::instruction{
	template <instruction_operator_id id>
	class unary_arithmetic : public base{
	public:
		template <typename... args_types>
		explicit unary_arithmetic(args_types &&... args)
			: base("", std::forward<args_types>(args)...){
			switch (id){
			case operator_id_type::not:
				name_ = "not";
				break;
			case operator_id_type::inc:
				name_ = "inc";
				break;
			case operator_id_type::dec:
				name_ = "dec";
				break;
			default:
				break;
			}
		}

		virtual void validate_operands() const override{
			if (operands_.size() != 1u)
				throw error_type::bad_operand_count;

			if (operands_[0]->id() != operand_id_type::register_value || operands_[0]->value_type() == value_type_id_type::float_)
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
			default:
				break;
			}

			throw error_type::ambiguous_operation;
		}

	protected:
		template <typename target_type>
		void execute_() const{
			switch (id){
			case operator_id_type::not:
				return operands_[0]->write(static_cast<target_type>(~operands_[0]->read<target_type>()));
			case operator_id_type::inc:
				return operands_[0]->write(static_cast<target_type>(operands_[0]->read<target_type>() + static_cast<target_type>(1)));
			case operator_id_type::dec:
				return operands_[0]->write(static_cast<target_type>(operands_[0]->read<target_type>() - static_cast<target_type>(1)));
			default:
				break;
			}

			throw error_type::bad_operation;
		}
	};

	template <instruction_operator_id id>
	class binary_arithmetic : public base{
	public:
		template <typename... args_types>
		explicit binary_arithmetic(args_types &&... args)
			: base("", std::forward<args_types>(args)...){
			switch (id){
			case operator_id_type::add:
				name_ = "add";
				break;
			case operator_id_type::sub:
				name_ = "sub";
				break;
			case operator_id_type::mult:
				name_ = "mult";
				break;
			case operator_id_type::div:
				name_ = "div";
				break;
			case operator_id_type::mod:
				name_ = "mod";
				break;
			case operator_id_type::and_:
				name_ = "and";
				break;
			case operator_id_type::xor_:
				name_ = "xor";
				break;
			case operator_id_type::or_:
				name_ = "or";
				break;
			case operator_id_type::sal:
				name_ = "sal";
				break;
			case operator_id_type::sar:
				name_ = "sar";
				break;
			default:
				break;
			}
		}

		virtual void apply_required_value_type() override{
			if (operands_.size() == 2u)
				operands_[1]->apply_value_type(operands_[0]->value_type());
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
				return execute_integral_<__int8>();
			case value_type_id_type::word:
				return execute_integral_<__int16>();
			case value_type_id_type::dword:
				return execute_integral_<__int32>();
			case value_type_id_type::qword:
				return execute_integral_<__int64>();
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
			switch (id){
			case operator_id_type::add:
				return target_()->write(static_cast<target_type>(left_()->read<target_type>() + right_()->read<target_type>()));
			case operator_id_type::sub:
				return target_()->write(static_cast<target_type>(left_()->read<target_type>() - right_()->read<target_type>()));
			case operator_id_type::mult:
				return target_()->write(static_cast<target_type>(left_()->read<target_type>() * right_()->read<target_type>()));
			case operator_id_type::div:
				return target_()->write(static_cast<target_type>(left_()->read<target_type>() / right_()->read<target_type>()));
			default:
				break;
			}

			throw error_type::bad_operation;
		}

		template <typename target_type>
		void execute_integral_() const{
			switch (id){
			case operator_id_type::mod:
				return target_()->write(static_cast<target_type>(left_()->read<target_type>() % right_()->read<target_type>()));
			case operator_id_type::and_:
				return target_()->write(static_cast<target_type>(left_()->read<target_type>() & right_()->read<target_type>()));
			case operator_id_type::xor_:
				return target_()->write(static_cast<target_type>(left_()->read<target_type>() ^ right_()->read<target_type>()));
			case operator_id_type::or_:
				return target_()->write(static_cast<target_type>(left_()->read<target_type>() | right_()->read<target_type>()));
			case operator_id_type::sal:
				return target_()->write(static_cast<target_type>(left_()->read<target_type>() << right_()->read<target_type>()));
			case operator_id_type::sar:
				return target_()->write(static_cast<target_type>(left_()->read<target_type>() >> right_()->read<target_type>()));
			default:
				break;
			}

			return execute_<target_type>();
		}

		virtual operand_ptr_type target_() const{
			return operands_[0];
		}

		virtual operand_ptr_type left_() const{
			return operands_[0];
		}

		virtual operand_ptr_type right_() const{
			return operands_[1];
		}
	};

	using not		= unary_arithmetic<instruction_operator_id::not>;
	using inc		= unary_arithmetic<instruction_operator_id::inc>;
	using dec		= unary_arithmetic<instruction_operator_id::dec>;

	using add		= binary_arithmetic<instruction_operator_id::add>;
	using sub		= binary_arithmetic<instruction_operator_id::sub>;

	using mult		= binary_arithmetic<instruction_operator_id::mult>;
	using div		= binary_arithmetic<instruction_operator_id::div>;
	using mod		= binary_arithmetic<instruction_operator_id::mod>;

	using and_		= binary_arithmetic<instruction_operator_id::and_>;
	using xor_		= binary_arithmetic<instruction_operator_id::xor_>;
	using or_		= binary_arithmetic<instruction_operator_id::or_>;

	using sal		= binary_arithmetic<instruction_operator_id::sal>;
	using sar		= binary_arithmetic<instruction_operator_id::sar>;
}

#endif /* !ELANG_ARITHMETIC_INSTRUCTION_H */
