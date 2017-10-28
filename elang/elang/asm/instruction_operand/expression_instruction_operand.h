#pragma once

#ifndef ELANG_EXPRESSION_INSTRUCTION_OPERAND_H
#define ELANG_EXPRESSION_INSTRUCTION_OPERAND_H

#include "constant_value_instruction_operand.h"

namespace elang{
	namespace easm{
		namespace instruction{
			class expression_operand : public operand_base{
			public:
				expression_operand(instruction_operator_id op, ptr_type left, ptr_type right)
					: op_(op), left_(left), right_(right){}

				virtual id_type id() const override{
					return id_type::expression;
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

				virtual void write_to_memory(char *buffer) override{
					eval_()->write_to_memory(buffer);
				}

				virtual void print(writer_type &writer, writer_type &wide_writer) const override{
					left_->print(writer, wide_writer);
					writer << " " << instruction_operator_utils::instruction_operator_id_to_string(op_) << " " << writer_type::manip_type::flush;
					right_->print(writer, wide_writer);
				}

				virtual void update_constant_value_type(value_type_id_type id) override{
					left_->update_constant_value_type(id);
					right_->update_constant_value_type(id);
				}

			protected:
				ptr_type eval_() const{
					switch (value_type()){
					case value_type_id_type::byte:
						return eval_integral_(left_->read<__int8>(), right_->read<__int8>());
					case value_type_id_type::word:
						return eval_integral_(left_->read<__int16>(), right_->read<__int16>());
					case value_type_id_type::dword:
						return eval_integral_(left_->read<__int32>(), right_->read<__int32>());
					case value_type_id_type::qword:
						return eval_integral_(left_->read<__int64>(), right_->read<__int64>());
					case value_type_id_type::float_:
						return eval_(left_->read<long double>(), right_->read<long double>());
					default:
						break;
					}

					throw instruction_error::ambiguous_operation;
				}

				template <typename value_type>
				ptr_type eval_(value_type left, value_type right) const{
					typedef constant_value_operand<value_type> constant_value_operand_type;
					switch (id){
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

				template <typename value_type>
				ptr_type eval_integral_(value_type left, value_type right) const{
					typedef constant_value_operand<value_type> constant_value_operand_type;
					switch (id){
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

					return eval_(left, right);
				}

				instruction_operator_id op_;
				ptr_type left_;
				ptr_type right_;
			};
		}
	}
}

#endif /* !ELANG_EXPRESSION_INSTRUCTION_OPERAND_H */
