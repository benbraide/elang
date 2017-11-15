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
			if (value_type_ != value_type_id_type::unknown)
				escape_();
		}

		explicit string_value_operand(std::string &&original)
			: value_type_(value_type_id_type::unknown), original_(std::move(original)){}

		virtual id_type id() const override{
			return id_type::constant_value;
		}

		virtual void apply_value_type(value_type_id_type type) override{
			if (value_type_ == value_type_id_type::unknown){
				value_type_ = type;
				escape_();
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
			if (value_.size() != size){//Perform conversion
				auto value = read_64bits();
				switch (size){
				case 1u:
					read_<__int8>(buffer);
					break;
				case 2u:
					read_<__int16>(buffer);
					break;
				case 4u:
					read_<__int32>(buffer);
					break;
				case 8u:
					if (type_id == numeric_type_id_type::float_)
						read_<long double>(buffer);
					else//Integral
						read_<__int64>(buffer);
					break;
				default:
					operand_base::read(buffer, size, type_id);
					break;
				}
			}
			else//No conversion necessary
				memcpy(buffer, value_.data(), std::min(size, elang::vm::machine_value_type_id_utils::machine_value_type_id_byte_size(value_type_)));
		}

		virtual uint64_type read_64bits() const override{
			if (value_type_ == value_type_id_type::word){//Wide
				if (value_.size() != 2u)//2 bytes required
					return operand_base::read_64bits();

				return static_cast<uint64_type>(read_<__int16>());
			}

			if (value_.size() != 1u)//1 byte required
				return operand_base::read_64bits();

			return static_cast<uint64_type>(value_[0]);
		}

		virtual void write_to_memory(char *buffer, uint64_type offset) const override{
			if (value_type_ == value_type_id_type::unknown)
				operand_base::write_to_memory(buffer, offset);
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
		template <typename target_type>
		target_type read_() const{
			auto value = target_type();
			memcpy(&value, value_.data(), std::min(sizeof(target_type), value_.size()));
			return value;
		}

		template <typename target_type>
		void read_(char *buffer) const{
			auto value = static_cast<target_type>(read_64bits());
			memcpy(buffer, &value, sizeof(target_type));
		}

		virtual void escape_(){
			if (value_type_ != value_type_id_type::word){
				elang::common::utils::escape_string(original_.begin(), original_.end(), value_);
				if (value_type_ != value_type_id_type::byte && value_.size() != 1u)
					throw error_type::ambiguous_operation;
			}
			else//Wide string
				escape_wide_();
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
