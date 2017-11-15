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

namespace elang::easm::instruction{
	class operand_base{
	public:
		typedef unsigned __int64 uint64_type;

		typedef operand_id id_type;
		typedef instruction_error error_type;
		typedef instruction_operator_id operator_id_type;

		typedef std::size_t size_type;
		typedef std::shared_ptr<operand_base> ptr_type;

		typedef elang::vm::machine_value_type_id value_type_id_type;
		typedef elang::common::output_writer writer_type;
		typedef elang::common::numeric_type_id numeric_type_id_type;

		virtual id_type id() const = 0;

		virtual void apply_value_type(value_type_id_type type){}

		virtual value_type_id_type value_type() const = 0;

		virtual bool is_constant() const{
			return false;
		}

		virtual bool is_float() const{
			return false;
		}

		virtual bool is_expression() const{
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

		virtual size_type memory_size() const{
			return 0;
		}

		virtual void print(writer_type &writer) const = 0;

		virtual void update_constant_value_type(value_type_id_type id){}

		template <typename value_type>
		void write(value_type value){
			write(reinterpret_cast<char *>(&value), sizeof(value_type), elang::common::numeric_type_to_id<value_type>::value);
		}

		virtual void write(const char *buffer, size_type size, numeric_type_id_type type_id){
			throw error_type::bad_operation;
		}

		template <typename target_type>
		target_type read() const{
			auto value = target_type();
			read(reinterpret_cast<char *>(&value), sizeof(target_type), elang::common::numeric_type_to_id<target_type>::value);
			return value;
		}

		virtual void read(char *buffer, size_type size, numeric_type_id_type type_id) const{
			throw error_type::bad_operation;
		}

		virtual uint64_type read_64bits() const{
			return read<uint64_type>();
		}

		virtual uint64_type effective_address() const{
			return read<uint64_type>();
		}

		virtual void push_onto_stack() const{
			throw error_type::bad_operation;
		}

		virtual void pop_from_stack(){
			throw error_type::bad_operation;
		}

		virtual void write_to_memory(char *buffer, uint64_type offset) const{
			throw error_type::bad_operation;
		}

		virtual void pre_write_to_memory(uint64_type offset) const{}
	};
}

#endif /* !ELANG_INSTRUCTION_OPERAND_BASE_H */
