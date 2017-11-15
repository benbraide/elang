#pragma once

#ifndef ELANG_GROUPED_EXPRESSION_INSTRUCTION_OPERAND_H
#define ELANG_GROUPED_EXPRESSION_INSTRUCTION_OPERAND_H

#include "instruction_operand_base.h"

namespace elang::easm::instruction{
	class grouped_expression_operand : public operand_base{
	public:
		explicit grouped_expression_operand(ptr_type value)
			: value_(value){}

		virtual id_type id() const override{
			return value_->id();
		}

		virtual void apply_value_type(value_type_id_type type) override{
			value_->apply_value_type(type);
		}

		virtual value_type_id_type value_type() const override{
			return value_->value_type();
		}

		virtual bool is_constant() const override{
			return value_->is_constant();
		}

		virtual bool is_expression() const override{
			return true;
		}

		virtual size_type instruction_bytes() const override{
			return value_->instruction_bytes();
		}

		virtual void read(char *buffer, size_type size, numeric_type_id_type type_id) const override{
			value_->read(buffer, size, type_id);
		}

		virtual uint64_type read_64bits() const override{
			return value_->read_64bits();
		}

		virtual void write_to_memory(char *buffer, uint64_type offset) const override{
			value_->write_to_memory(buffer, offset);
		}

		virtual void print(writer_type &writer) const override{
			writer.write("(");
			value_->print(writer);
			writer.write(")");
		}

		virtual void update_constant_value_type(value_type_id_type id) override{
			value_->update_constant_value_type(id);
		}

	protected:
		ptr_type value_;
	};
}

#endif /* !ELANG_GROUPED_EXPRESSION_INSTRUCTION_OPERAND_H */
