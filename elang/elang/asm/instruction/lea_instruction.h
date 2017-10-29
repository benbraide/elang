#pragma once

#ifndef ELANG_LEA_INSTRUCTION_H
#define ELANG_LEA_INSTRUCTION_H

#include "instruction_base.h"

namespace elang{
	namespace easm{
		namespace instruction{
			class lea : public base{
			public:
				template <typename... args_types>
				explicit lea(args_types &&... args)
					: base("lea", std::forward<args_types>(args)...){}

				virtual value_type_id_type required_value_type() const override{
					return value_type_id_type::qword;
				}

				virtual void validate_operands() const override{
					if (operands_.size() != 2u)
						throw error_type::bad_operand_count;

					if (operands_[0]->id() != operand_id_type::register_value)
						throw error_type::bad_operand;

					if (operands_[0]->value_type() != value_type_id_type::qword || operands_[1]->value_type() != value_type_id_type::qword)
						throw error_type::operands_type_mismatch;
				}

				virtual void execute() const override{
					operands_[0]->write(operands_[1]->effective_address());
				}
			};
		}
	}
}

#endif /* !ELANG_LEA_INSTRUCTION_H */
