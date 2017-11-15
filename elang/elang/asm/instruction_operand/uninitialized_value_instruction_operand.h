#pragma once

#ifndef ELANG_UNINITIALIZED_VALUE_INSTRUCTION_OPERAND_H
#define ELANG_UNINITIALIZED_VALUE_INSTRUCTION_OPERAND_H

#include "instruction_operand_base.h"

namespace elang::easm::instruction{
	class uninitialized_value_operand : public operand_base{
	public:
		virtual id_type id() const override{
			return id_type::constant_value;
		}

		virtual void apply_value_type(value_type_id_type type) override{
			value_type_ = type;
		}

		virtual value_type_id_type value_type() const override{
			return value_type_;
		}

		virtual bool is_constant() const override{
			return true;
		}

		virtual void read(char *buffer, size_type size, numeric_type_id_type type_id) const override{
			throw error_type::bad_operand;
		}

		virtual uint64_type read_64bits() const override{
			throw error_type::bad_operand;
		}

		virtual void write_to_memory(char *buffer, uint64_type offset) const override{}

		virtual void print(writer_type &writer) const override{
			writer.write("?");
		}

	protected:
		value_type_id_type value_type_;
	};
}

#endif /* !ELANG_UNINITIALIZED_VALUE_INSTRUCTION_OPERAND_H */
