#pragma once

#ifndef ELANG_NOP_INSTRUCTION_H
#define ELANG_NOP_INSTRUCTION_H

#include "instruction_base.h"

namespace elang{
	namespace easm{
		namespace instruction{
			class nop : public base{
			public:
				template <typename... args_types>
				explicit nop(args_types &&... args)
					: base("nop", std::forward<args_types>(args)...){}

				virtual void validate_operands() const override{
					if (!operands_.empty())
						throw error_type::bad_operand_count;
				}

				virtual void execute() const override{}
			};
		}
	}
}

#endif /* !ELANG_NOP_INSTRUCTION_H */
