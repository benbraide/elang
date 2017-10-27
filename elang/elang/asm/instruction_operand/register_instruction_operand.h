#pragma once

#ifndef ELANG_REGISTER_INSTRUCTION_OPERAND_H
#define ELANG_REGISTER_INSTRUCTION_OPERAND_H

#include "../../vm/machine_register.h"

#include "instruction_operand_base.h"

namespace elang{
	namespace asm_{
		namespace instruction{
			class register_operand : public operand_base{
			public:
				typedef elang::vm::machine_register register_type;

				explicit register_operand(register_type &value)
					: value_(&value){}

				virtual id_type id() const override{
					return id_type::register_value;
				}

				virtual value_type_id_type value_type() const override{
					return value_->type_id();
				}

				virtual void write(const char *buffer, size_type size, numeric_type_id_type type_id) override{
					switch (type_id){
					case numeric_type_id_type::int8:
						write_<elang::common::numeric_id_to_type<numeric_type_id_type::int8>::type>(buffer);
						break;
					case numeric_type_id_type::uint8:
						write_<elang::common::numeric_id_to_type<numeric_type_id_type::uint8>::type>(buffer);
						break;
					case numeric_type_id_type::int16:
						write_<elang::common::numeric_id_to_type<numeric_type_id_type::int16>::type>(buffer);
						break;
					case numeric_type_id_type::uint16:
						write_<elang::common::numeric_id_to_type<numeric_type_id_type::uint16>::type>(buffer);
						break;
					case numeric_type_id_type::int32:
						write_<elang::common::numeric_id_to_type<numeric_type_id_type::int32>::type>(buffer);
						break;
					case numeric_type_id_type::uint32:
						write_<elang::common::numeric_id_to_type<numeric_type_id_type::uint32>::type>(buffer);
						break;
					case numeric_type_id_type::int64:
						write_<elang::common::numeric_id_to_type<numeric_type_id_type::int64>::type>(buffer);
						break;
					case numeric_type_id_type::uint64:
						write_<elang::common::numeric_id_to_type<numeric_type_id_type::uint64>::type>(buffer);
						break;
					case numeric_type_id_type::float_:
						write_<elang::common::numeric_id_to_type<numeric_type_id_type::float_>::type>(buffer);
						break;
					case numeric_type_id_type::double_:
						write_<elang::common::numeric_id_to_type<numeric_type_id_type::double_>::type>(buffer);
						break;
					case numeric_type_id_type::long_double_:
						write_<elang::common::numeric_id_to_type<numeric_type_id_type::long_double_>::type>(buffer);
						break;
					default:
						operand_base::write(buffer, size, type_id);
						break;
					}
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

				virtual void print(writer_type &writer, writer_type &wide_writer) const override{
					writer << *value_ << writer_type::manip_type::flush;
				}

				virtual void push_onto_stack() override{
					value_->push_onto_stack();
				}

				virtual void pop_from_stack() override{
					value_->pop_from_stack();
				}

			protected:
				template <typename value_type>
				void write_(const char *buffer){
					value_->write(*reinterpret_cast<const value_type *>(buffer));
				}

				template <typename target_type>
				void read_(char *buffer) const{
					*reinterpret_cast<target_type *>(buffer) = value_->read<target_type>();
				}

				register_type *value_;
			};
		}
	}
}

#endif /* !ELANG_REGISTER_INSTRUCTION_OPERAND_H */
