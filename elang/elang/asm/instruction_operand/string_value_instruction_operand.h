#pragma once

#ifndef ELANG_STRING_VALUE_INSTRUCTION_OPERAND_H
#define ELANG_STRING_VALUE_INSTRUCTION_OPERAND_H

#include "../../common/common_utils.h"

#include "instruction_operand_base.h"

namespace elang::easm::instruction{
	class string_value_operand : public operand_base{
	public:
		using operand_base::read;

		string_value_operand(value_type_id_type value_type_id, std::string &&original)
			: value_type_(value_type_id), original_(std::move(original)){
			if (value_type_id <= value_type_id_type::word)
				escape_();
			else//Reject id
				value_type_ = value_type_id_type::unknown;
		}

		explicit string_value_operand(std::string &&original)
			: value_type_(value_type_id_type::unknown), original_(std::move(original)){}

		virtual id_type id() const override{
			return id_type::constant_value;
		}

		virtual void apply_value_type(value_type_id_type type) override{
			if (value_type_ == value_type_id_type::unknown){
				if (type <= value_type_id_type::word){
					value_type_ = type;
					escape_();
				}
				else//Error
					throw error_type::ambiguous_operation;
			}
		}

		virtual value_type_id_type value_type() const override{
			return value_type_;
		}

		virtual bool is_constant() const override{
			return true;
		}

		virtual size_type instruction_bytes() const override{
			if (value_type_ == value_type_id_type::unknown)
				return operand_base::instruction_bytes();
			return (value_.size() * elang::vm::machine_value_type_id_utils::machine_value_type_id_byte_size(value_type_));
		}

		virtual void read(char *buffer, size_type size, numeric_type_id_type type_id) const override{
			if (value_.size() == elang::vm::machine_value_type_id_utils::machine_value_type_id_byte_size(value_type_))
				memcpy(buffer, value_.data(), std::min(size, elang::vm::machine_value_type_id_utils::machine_value_type_id_byte_size(value_type_)));
			else//Single character required
				operand_base::read(buffer, size, type_id);
		}

		virtual uint64_type read_64bits() const override{
			if (value_.size() == elang::vm::machine_value_type_id_utils::machine_value_type_id_byte_size(value_type_)){//Single character required
				if (value_type_ == value_type_id_type::byte)
					return static_cast<uint64_type>(value_[0]);
				return static_cast<uint64_type>(read<__int16>());
			}
			return operand_base::read_64bits();
		}

		virtual void write_to_memory(char *buffer) const override{
			if (value_type_ == value_type_id_type::unknown)
				operand_base::write_to_memory(buffer);
			else//Write bytes
				memcpy(buffer, value_.data(), value_.size());
		}

		virtual void print(writer_type &writer) const override{
			writer.write("'").write(original_).write("'");
		}

		virtual void update_constant_value_type(value_type_id_type id) override{
			value_type_ = id;
		}

	protected:
		virtual void escape_(){
			if (value_type_ == value_type_id_type::word)//Wide string
				escape_wide_();
			else//Narrow string
				elang::common::utils::escape_string(original_.begin(), original_.end(), value_);
		}

		virtual void escape_wide_(){
			std::wstring wvalue;
			elang::common::utils::escape_string(original_.begin(), original_.end(), wvalue);
			value_.assign(reinterpret_cast<const char *>(wvalue.data()), wvalue.size() * sizeof(wchar_t));
		}

		value_type_id_type value_type_;
		std::string original_;
		std::string value_;
	};
}

#endif /* !ELANG_STRING_VALUE_INSTRUCTION_OPERAND_H */
