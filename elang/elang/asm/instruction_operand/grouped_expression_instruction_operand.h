#pragma once

#ifndef ELANG_GROUPED_EXPRESSION_INSTRUCTION_OPERAND_H
#define ELANG_GROUPED_EXPRESSION_INSTRUCTION_OPERAND_H

#include "instruction_operand_base.h"

namespace elang{
	namespace easm{
		namespace instruction{
			class grouped_expression_operand : public operand_base{
			public:
				explicit grouped_expression_operand(ptr_type value)
					: value_(value){}

				virtual id_type id() const override{
					return value_->id();
				}

				virtual value_type_id_type value_type() const override{
					return value_->value_type();
				}

				virtual bool is_constant() const override{
					return value_->is_constant();
				}

				virtual size_type instruction_bytes() const override{
					return value_->instruction_bytes();
				}

				virtual void read(char *buffer, size_type size, numeric_type_id_type type_id) const override{
					value_->read(buffer, size, type_id);
				}

				virtual void write_to_memory(char *buffer) override{
					value_->write_to_memory(buffer);
				}

				virtual void print(writer_type &writer, writer_type &wide_writer) const override{
					writer << "(" << writer_type::manip_type::flush;
					value_->print(writer, wide_writer);
					writer << ")" << writer_type::manip_type::flush;
				}

				virtual void update_constant_value_type(value_type_id_type id) override{
					value_->update_constant_value_type(id);
				}

			protected:
				ptr_type value_;
			};
		}
	}
}

#endif /* !ELANG_GROUPED_EXPRESSION_INSTRUCTION_OPERAND_H */
