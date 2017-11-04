#pragma once

#ifndef ELANG_INSTRUCTION_BASE_H
#define ELANG_INSTRUCTION_BASE_H

#include <vector>

#include "../../vm/machine_register.h"

#include "../instruction_operand/instruction_operand_base.h"

namespace elang{
	namespace easm{
		namespace instruction{
			class base{
			public:
				typedef unsigned __int64 uint64_type;

				typedef instruction_error error_type;
				typedef instruction_operator_id operator_id_type;

				typedef std::size_t size_type;
				typedef std::shared_ptr<base> ptr_type;

				typedef operand_id operand_id_type;
				typedef operand_base::ptr_type operand_ptr_type;
				typedef std::vector<operand_ptr_type> operand_ptr_list_type;

				typedef elang::vm::machine_value_type_id value_type_id_type;
				typedef elang::vm::machine_register register_type;

				typedef elang::common::output_writer writer_type;
				typedef elang::common::numeric_type_id numeric_type_id_type;

				template <typename list_type>
				base(const std::string &name, list_type &&operands)
					: name_(name), operands_(std::forward<list_type>(operands)), instruction_bytes_(0u){}

				virtual value_type_id_type required_value_type() const{
					return value_type_id_type::unknown;
				}

				virtual size_type instruction_bytes() const{
					if (instruction_bytes_ != 0u)
						return instruction_bytes_;

					size_type bytes = 0u;
					for (auto item : operands_)
						bytes += item->instruction_bytes();

					return (instruction_bytes_ = (bytes + this_instruction_bytes_()));
				}

				virtual void apply_required_value_type(){
					for (auto entry : operands_)
						entry->apply_value_type(required_value_type());
				}

				virtual void validate_operands() const{}

				virtual void write_memory(uint64_type &address) const{
					address += instruction_bytes();
				}

				virtual void execute_and_update_instruction_pointer(register_type &instruction_pointer) const{
					execute();
					instruction_pointer.inc<uint64_type>(instruction_bytes());//Advance Instruction Pointer
				}

				virtual void execute() const = 0;

				virtual void print(writer_type &writer, writer_type &wide_writer) const{
					writer << name_ << " " << writer_type::manip_type::flush;

					auto is_first = true;
					for (auto item : operands_){
						if (is_first)//No delimiter needed
							is_first = false;
						else//Print delimiter
							writer << ", " << writer_type::manip_type::flush;

						item->print(writer, wide_writer);
					}
				}

			protected:
				virtual size_type this_instruction_bytes_() const{
					return 2u;
				}

				std::string name_;
				operand_ptr_list_type operands_;
				mutable size_type instruction_bytes_;
			};
		}
	}
}

#endif /* !ELANG_INSTRUCTION_BASE_H */
