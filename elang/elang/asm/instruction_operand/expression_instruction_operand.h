#pragma once

#ifndef ELANG_EXPRESSION_INSTRUCTION_OPERAND_H
#define ELANG_EXPRESSION_INSTRUCTION_OPERAND_H

#include "constant_value_instruction_operand.h"

namespace elang::easm::instruction{
	class expression_operand : public operand_base{
	public:
		expression_operand(instruction_operator_id op, ptr_type left, ptr_type right)
			: op_(op), left_(left), right_(right){}

		virtual id_type id() const override{
			return id_type::expression;
		}

		virtual void apply_value_type(value_type_id_type type) override{
			left_->apply_value_type(type);
			right_->apply_value_type(type);
		}

		virtual value_type_id_type value_type() const override{
			return left_->value_type();
		}

		virtual bool is_constant() const override{
			return (left_->is_constant() && right_->is_constant());
		}

		virtual size_type instruction_bytes() const override{
			return left_->instruction_bytes();
		}

		virtual void read(char *buffer, size_type size, numeric_type_id_type type_id) const override{
			eval_()->read(buffer, size, type_id);
		}

		virtual uint64_type read_64bits() const override{
			return eval_()->read_64bits();
		}

		virtual void write_to_memory(char *buffer, uint64_type offset) const override{
			left_->pre_write_to_memory(offset);
			right_->pre_write_to_memory(offset);
			eval_()->write_to_memory(buffer, offset);
		}

		virtual void print(writer_type &writer) const override{
			left_->print(writer);
			writer.write(" ").write(instruction_operator_utils::instruction_operator_id_to_string(op_)).write(" ");
			right_->print(writer);
		}

		virtual void update_constant_value_type(value_type_id_type id) override{
			left_->update_constant_value_type(id);
			right_->update_constant_value_type(id);
		}

	protected:
		ptr_type eval_() const{
			switch (value_type()){
			case value_type_id_type::byte:
				return eval_integral_<unsigned __int8>();
			case value_type_id_type::word:
				return eval_integral_<unsigned __int16>();
			case value_type_id_type::dword:
				return eval_integral_<unsigned __int32>();
			case value_type_id_type::qword:
				return eval_integral_<unsigned __int64>();
			case value_type_id_type::float_:
				return eval_general_<long double>();
			default:
				break;
			}

			throw instruction_error::ambiguous_operation;
		}

		template <typename arg_value_type>
		ptr_type eval_general_() const{
			typedef constant_value_operand<arg_value_type> constant_value_operand_type;

			arg_value_type left, right;
			if (std::is_floating_point<arg_value_type>::value){
				if (left_->is_float())
					left = static_cast<arg_value_type>(left_->read<long double>());
				else//Convert integral to float
					left = static_cast<arg_value_type>(left_->read_64bits());

				if (right_->is_float())
					right = static_cast<arg_value_type>(right_->read<long double>());
				else//Convert integral to float
					right = static_cast<arg_value_type>(right_->read_64bits());
			}
			else{//Integral
				left = static_cast<arg_value_type>(left_->read_64bits());
				right = static_cast<arg_value_type>(right_->read_64bits());
			}

			switch (op_){
			case instruction_operator_id::add:
				return std::make_shared<constant_value_operand_type>(value_type(), left + right);
			case instruction_operator_id::sub:
				return std::make_shared<constant_value_operand_type>(value_type(), left - right);
			case instruction_operator_id::mult:
				return std::make_shared<constant_value_operand_type>(value_type(), left * right);
			case instruction_operator_id::div:
				return std::make_shared<constant_value_operand_type>(value_type(), left / right);
			default:
				break;
			}

			throw instruction_error::bad_operation;
		}

		template <typename arg_value_type>
		ptr_type eval_integral_() const{
			typedef constant_value_operand<arg_value_type> constant_value_operand_type;

			auto left = static_cast<arg_value_type>(left_->read_64bits());
			auto right = static_cast<arg_value_type>(right_->read_64bits());

			switch (op_){
			case instruction_operator_id::mod:
				return std::make_shared<constant_value_operand_type>(value_type(), left % right);
			case instruction_operator_id::and_:
				return std::make_shared<constant_value_operand_type>(value_type(), left & right);
			case instruction_operator_id::or_:
				return std::make_shared<constant_value_operand_type>(value_type(), left | right);
			case instruction_operator_id::xor_:
				return std::make_shared<constant_value_operand_type>(value_type(), left ^ right);
			case instruction_operator_id::sal:
				return std::make_shared<constant_value_operand_type>(value_type(), left << right);
			case instruction_operator_id::sar:
				return std::make_shared<constant_value_operand_type>(value_type(), left >> right);
			default:
				break;
			}

			return eval_general_<arg_value_type>();
		}

		instruction_operator_id op_;
		ptr_type left_;
		ptr_type right_;
	};
}

#endif /* !ELANG_EXPRESSION_INSTRUCTION_OPERAND_H */
