#pragma once

#ifndef ELANG_LABEL_INSTRUCTION_OPERAND_H
#define ELANG_LABEL_INSTRUCTION_OPERAND_H

#include "constant_value_instruction_operand.h"

namespace elang{
	namespace easm{
		namespace instruction{
			class label_operand : public constant_value_operand<unsigned __int64>{
			public:
				typedef constant_value_operand<unsigned __int64> base_type;

				explicit label_operand(const std::string &value)
					: base_type(value_type_id_type::qword), value_(value){}

				virtual id_type id() const override{
					return id_type::label;
				}

				virtual void write_to_memory(char *buffer) override{
					throw error_type::bad_operation;
				}

				virtual void print(writer_type &writer, writer_type &wide_writer) const override{
					writer << value_ << writer_type::manip_type::flush;
				}

				virtual void update_constant_value_type(value_type_id_type id) override{}

			protected:
				std::string value_;
				std::string resolved_;
			};

			class qualified_label_operand : public label_operand{
			public:
				explicit qualified_label_operand(const std::string &value)
					: label_operand(value){}
			};
		}
	}
}

#endif /* !ELANG_LABEL_INSTRUCTION_OPERAND_H */
