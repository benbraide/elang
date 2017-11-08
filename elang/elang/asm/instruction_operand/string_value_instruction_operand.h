#pragma once

#ifndef ELANG_STRING_VALUE_INSTRUCTION_OPERAND_H
#define ELANG_STRING_VALUE_INSTRUCTION_OPERAND_H

#include "instruction_operand_base.h"

namespace elang::easm::instruction{
	class string_value_operand : public operand_base{
	public:
		explicit string_value_operand(value_type_id_type value_type_id, std::string &&original, std::string &&value)
			: value_type_(value_type_id), original_(std::move(original)), value_(std::move(value)){}

		explicit string_value_operand(std::string &&original, std::string &&value)
			: value_type_(value_type_id_type::unknown), original_(std::move(original)), value_(std::move(value)){}

		virtual id_type id() const override{
			return id_type::constant_value;
		}

		virtual void apply_value_type(value_type_id_type type) override{
			if (value_type_ == value_type_id_type::unknown)
				value_type_ = type;
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

			auto char_count = value_.size();
			auto byte_size = elang::vm::machine_value_type_id_utils::machine_value_type_id_byte_size(value_type_);
			auto overflow = (char_count % byte_size);

			return ((overflow == 0u) ? (char_count) : (char_count + (byte_size - overflow)));//Ensure value is a multiple of 'byte_size'
		}

		virtual void read(char *buffer, size_type size, numeric_type_id_type type_id) const override{
			if (value_.size() == 1u)
				*buffer = value_[0];
			else//Single character required
				operand_base::read_64bits();
		}

		virtual uint64_type read_64bits() const override{
			if (value_.size() == 1u)//Single character required
				return static_cast<uint64_type>(value_[0]);
			return operand_base::read_64bits();
		}

		virtual void write_to_memory(char *buffer) const override{
			if (value_type_ == value_type_id_type::unknown)
				operand_base::write_to_memory(buffer);
			else//Write bytes
				memcpy(buffer, value_.data(), value_.size());
		}

		virtual void print(writer_type &writer, writer_type &wide_writer) const override{
			writer << "'" << original_ << "'" << writer_type::manip_type::flush;
		}

		virtual void update_constant_value_type(value_type_id_type id) override{
			value_type_ = id;
		}

	protected:
		value_type_id_type value_type_;
		std::string original_;
		std::string value_;
	};
}

#endif /* !ELANG_STRING_VALUE_INSTRUCTION_OPERAND_H */
