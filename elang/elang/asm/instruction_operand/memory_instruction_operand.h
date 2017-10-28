#pragma once

#ifndef ELANG_MEMORY_INSTRUCTION_OPERAND_H
#define ELANG_MEMORY_INSTRUCTION_OPERAND_H

#include "../../vm/machine.h"

#include "instruction_operand_base.h"

namespace elang{
	namespace easm{
		namespace instruction{
			class memory_operand : public operand_base{
			public:
				memory_operand(value_type_id_type value_type_id, ptr_type value)
					: value_type_id_(value_type_id), value_(value){}

				virtual id_type id() const override{
					return id_type::memory;
				}

				virtual value_type_id_type value_type() const override{
					return value_type_id_;
				}

				virtual void write(const char *buffer, size_type size, numeric_type_id_type type_id) override{
					elang::vm::machine::memory_manager.write(value_->read<uint64_type>(), buffer, size);
				}

				virtual void read(char *buffer, size_type size, numeric_type_id_type type_id) const override{
					elang::vm::machine::memory_manager.read(value_->read<uint64_type>(), buffer, size);
				}

				virtual uint64_type effective_address() const override{
					return value_->read<uint64_type>();
				}

				virtual void print(writer_type &writer, writer_type &wide_writer) const override{
					if (value_type_id_ != value_type_id_type::unknown)
						writer << elang::vm::machine_value_type_id_utils::machine_value_type_id_to_string(value_type_id_);

					writer << "[" << writer_type::manip_type::flush;
					value_->print(writer, wide_writer);
					writer << "]" << writer_type::manip_type::flush;
				}

			protected:
				value_type_id_type value_type_id_;
				ptr_type value_;
			};
		}
	}
}

#endif /* !ELANG_MEMORY_INSTRUCTION_OPERAND_H */
