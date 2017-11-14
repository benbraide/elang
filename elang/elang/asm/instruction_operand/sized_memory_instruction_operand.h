#pragma once

#ifndef ELANG_SIZED_MEMORY_INSTRUCTION_OPERAND_H
#define ELANG_SIZED_MEMORY_INSTRUCTION_OPERAND_H

#include "../../vm/machine.h"

#include "instruction_operand_base.h"

namespace elang::easm::instruction{
	class sized_memory_operand : public operand_base{
	public:
		typedef elang::vm::runtime_type runtime_type;

		sized_memory_operand(runtime_type &type, ptr_type value)
			: type_(&type), value_(value){}

		virtual id_type id() const override{
			return id_type::memory;
		}

		virtual value_type_id_type value_type() const override{
			return value_type_id_type::unknown;
		}

		virtual size_type memory_size() const override{
			return type_->size();
		}

		virtual void write(const char *buffer, size_type size, numeric_type_id_type type_id) override{
			elang::vm::machine::memory_manager.write(value_->read_64bits(), buffer, size);
		}

		virtual void read(char *buffer, size_type size, numeric_type_id_type type_id) const override{
			elang::vm::machine::memory_manager.read(value_->read<uint64_type>(), buffer, size);
		}

		virtual uint64_type effective_address() const override{
			return value_->read<uint64_type>();
		}

		virtual void print(writer_type &writer) const override{
			writer.write("[");
			value_->print(writer);
			writer.write("]");
			type_->print(writer);
		}

	protected:
		runtime_type *type_;
		ptr_type value_;
	};
}

#endif /* !ELANG_SIZED_MEMORY_INSTRUCTION_OPERAND_H */
