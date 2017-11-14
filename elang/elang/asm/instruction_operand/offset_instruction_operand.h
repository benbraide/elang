#pragma once

#ifndef ELANG_OFFSET_INSTRUCTION_OPERAND_H
#define ELANG_OFFSET_INSTRUCTION_OPERAND_H

#include "constant_value_instruction_operand.h"

namespace elang::easm::instruction{
	class offset_operand : public constant_value_operand<unsigned __int64>{
	public:
		typedef constant_value_operand<unsigned __int64> base_type;

		offset_operand()
			: base_type(value_type_id_type::unknown){}

		virtual id_type id() const override{
			return id_type::offset;
		}

		virtual bool is_constant() const override{
			return true;
		}

		virtual void write_to_memory(char *buffer, uint64_type offset) const override{
			pre_write_to_memory(offset);
			base_type::write_to_memory(buffer, offset);
		}

		virtual void pre_write_to_memory(uint64_type offset) const override{
			const_cast<offset_operand *>(this)->value_ = offset;
		}

		virtual void print(writer_type &writer) const override{
			writer.write("$");
		}

		virtual void update_constant_value_type(value_type_id_type id) override{}
	};
}

#endif /* !ELANG_OFFSET_INSTRUCTION_OPERAND_H */
