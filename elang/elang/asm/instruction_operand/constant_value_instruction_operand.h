#pragma once

#ifndef ELANG_CONSTANT_VALUE_INSTRUCTION_OPERAND_H
#define ELANG_CONSTANT_VALUE_INSTRUCTION_OPERAND_H

#include "instruction_operand_base.h"

namespace elang{
	namespace easm{
		namespace instruction{
			template <class operand_value_type>
			class constant_value_operand : public operand_base{
			public:
				typedef operand_value_type operand_value_type;

				explicit constant_value_operand(value_type_id_type value_type_id, operand_value_type value = operand_value_type())
					: value_type_(value_type_id), value_(value){}

				explicit constant_value_operand(operand_value_type value = operand_value_type())
					: value_type_(value_type_id_type::unknown), value_(value){}

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

				virtual void read(char *buffer, size_type size, numeric_type_id_type type_id) const override{
					switch (type_id){
					case numeric_type_id_type::int8:
						read_<elang::common::numeric_id_to_type<numeric_type_id_type::int8>::type>(buffer);
						break;
					case numeric_type_id_type::uint8:
						read_<elang::common::numeric_id_to_type<numeric_type_id_type::uint8>::type>(buffer);
						break;
					case numeric_type_id_type::int16:
						read_<elang::common::numeric_id_to_type<numeric_type_id_type::int16>::type>(buffer);
						break;
					case numeric_type_id_type::uint16:
						read_<elang::common::numeric_id_to_type<numeric_type_id_type::uint16>::type>(buffer);
						break;
					case numeric_type_id_type::int32:
						read_<elang::common::numeric_id_to_type<numeric_type_id_type::int32>::type>(buffer);
						break;
					case numeric_type_id_type::uint32:
						read_<elang::common::numeric_id_to_type<numeric_type_id_type::uint32>::type>(buffer);
						break;
					case numeric_type_id_type::int64:
						read_<elang::common::numeric_id_to_type<numeric_type_id_type::int64>::type>(buffer);
						break;
					case numeric_type_id_type::uint64:
						read_<elang::common::numeric_id_to_type<numeric_type_id_type::uint64>::type>(buffer);
						break;
					case numeric_type_id_type::float_:
						read_<elang::common::numeric_id_to_type<numeric_type_id_type::float_>::type>(buffer);
						break;
					case numeric_type_id_type::double_:
						read_<elang::common::numeric_id_to_type<numeric_type_id_type::double_>::type>(buffer);
						break;
					case numeric_type_id_type::long_double_:
						read_<elang::common::numeric_id_to_type<numeric_type_id_type::long_double_>::type>(buffer);
						break;
					default:
						operand_base::read(buffer, size, type_id);
						break;
					}
				}

				virtual uint64_type read_64bits() const override{
					return static_cast<uint64_type>(value_);
				}

				virtual void write_to_memory(char *buffer) const override{
					switch (value_type_){
					case value_type_id_type::byte:
						write_to_memory_<__int8>(buffer);
						break;
					case value_type_id_type::word:
						write_to_memory_<__int16>(buffer);
						break;
					case value_type_id_type::dword:
						write_to_memory_<__int32>(buffer);
						break;
					case value_type_id_type::qword:
						write_to_memory_<__int64>(buffer);
						break;
					case value_type_id_type::float_:
						write_to_memory_<long double>(buffer);
						break;
					default:
						operand_base::write_to_memory(buffer);
						break;
					}
				}

				virtual void print(writer_type &writer, writer_type &wide_writer) const override{
					writer << value_ << writer_type::manip_type::flush;
				}

				virtual void update_constant_value_type(value_type_id_type id) override{
					value_type_ = id;
				}

			protected:
				template <typename target_type>
				void read_(char *buffer) const{
					auto value = static_cast<target_type>(value_);
					memcpy(buffer, &value, sizeof(target_type));
				}

				template <typename target_type>
				void write_to_memory_(char *buffer) const{
					auto value = static_cast<target_type>(value_);
					memcpy(buffer, &value, sizeof(target_type));
				}

				value_type_id_type value_type_;
				operand_value_type value_;
			};
		}
	}
}

#endif /* !ELANG_CONSTANT_VALUE_INSTRUCTION_OPERAND_H */
