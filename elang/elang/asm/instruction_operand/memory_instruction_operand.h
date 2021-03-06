#pragma once

#ifndef ELANG_MEMORY_INSTRUCTION_OPERAND_H
#define ELANG_MEMORY_INSTRUCTION_OPERAND_H

#include "../../vm/machine.h"

#include "instruction_operand_base.h"

namespace elang::easm::instruction{
	class memory_operand : public operand_base{
	public:
		memory_operand(value_type_id_type value_type_id, ptr_type value)
			: value_type_id_(value_type_id), value_(value){}

		virtual id_type id() const override{
			return id_type::memory;
		}

		virtual void apply_value_type(value_type_id_type type) override{
			if (value_type_id_ == value_type_id_type::unknown)
				value_type_id_ = type;
		}

		virtual value_type_id_type value_type() const override{
			return value_type_id_;
		}

		virtual size_type memory_size() const override{
			switch (value_type_id_){
			case value_type_id_type::byte:
				return 1u;
			case value_type_id_type::word:
				return 2u;
			case value_type_id_type::dword:
				return 4u;
			case value_type_id_type::qword:
			case value_type_id_type::float_:
				return 8u;
			default:
				break;
			}

			return 0u;
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
			if (value_type_id_ != value_type_id_type::unknown)
				writer.write(elang::vm::machine_value_type_id_utils::machine_value_type_id_to_string(value_type_id_)).write(" PTR");

			writer.write("[");
			value_->print(writer);
			writer.write("]");
		}

		virtual void push_onto_stack() const override{
			auto size = memory_size();
			if (size != 0u)
				elang::vm::machine::runtime.stack.push(value_->read<uint64_type>(), size);
			else//Error
				throw error_type::ambiguous_operation;
		}

		virtual void pop_from_stack() override{
			auto size = memory_size();
			if (size != 0u)
				elang::vm::machine::runtime.stack.pop(value_->read<uint64_type>(), size);
			else//Error
				throw error_type::ambiguous_operation;
		}

	protected:
		value_type_id_type value_type_id_;
		ptr_type value_;
	};
}

#endif /* !ELANG_MEMORY_INSTRUCTION_OPERAND_H */
