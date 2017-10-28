#pragma once

#ifndef ELANG_INSTRUCTION_OPERAND_BASE_H
#define ELANG_INSTRUCTION_OPERAND_BASE_H

#include <memory>

#include "../../vm/machine_value_type_id.h"

#include "../../common/output_writer.h"
#include "../../common/numeric_type_utils.h"

#include "../instruction_error.h"
#include "../instruction_operator_id.h"

#include "instruction_operand_id.h"

namespace elang{
	namespace easm{
		namespace instruction{
			class operand_base{
			public:
				typedef operand_id id_type;
				typedef instruction_error error_type;
				typedef instruction_operator_id operator_id_type;

				typedef std::size_t size_type;
				typedef std::shared_ptr<operand_base> ptr_type;

				typedef elang::vm::machine_value_type_id value_type_id_type;
				typedef elang::common::output_writer writer_type;
				typedef elang::common::numeric_type_id numeric_type_id_type;

				virtual id_type id() const = 0;

				virtual value_type_id_type value_type() const = 0;

				virtual bool is_constant() const{
					return false;
				}

				virtual size_type instruction_bytes() const{
					switch (value_type()){
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

					throw instruction_error::ambiguous_operation;
				}

				virtual void print(writer_type &writer, writer_type &wide_writer) const = 0;

				virtual void update_constant_value_type(value_type_id_type id){}

				template <typename value_type>
				void write(value_type value){
					write(reinterpret_cast<char *>(&value), sizeof(value_type));
				}

				virtual void write(const char *buffer, size_type size, numeric_type_id_type type_id) = 0;

				template <typename target_type>
				target_type read() const{
					auto value = target_type();
					read(reinterpret_cast<char *>(&value), sizeof(target_type));
					return value;
				}

				virtual void read(char *buffer, size_type size, numeric_type_id_type type_id) const = 0;

				virtual void push_onto_stack(){
					throw error_type::bad_operation;
				}

				virtual void pop_from_stack(){
					throw error_type::bad_operation;
				}

				virtual void write_to_memory(char *buffer){
					throw error_type::bad_operation;
				}
			};
		}
	}
}

#endif /* !ELANG_INSTRUCTION_OPERAND_BASE_H */
