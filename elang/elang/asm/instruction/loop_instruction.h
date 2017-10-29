#pragma once

#ifndef ELANG_LOOP_INSTRUCTION_H
#define ELANG_LOOP_INSTRUCTION_H

#include "../../vm/machine.h"

#include "instruction_base.h"

namespace elang{
	namespace easm{
		namespace instruction{
			class loop : public base{
			public:
				template <typename... args_types>
				explicit loop(args_types &&... args)
					: base("loop", std::forward<args_types>(args)...){}

				virtual void validate_operands() const override{
					if (operands_.size() != 1u)
						throw error_type::bad_operand_count;

					if (operands_[0]->id() != operand_id_type::label)
						throw error_type::bad_operand;
				}

				virtual void execute_and_update_instruction_pointer(register_type &instruction_pointer) const override{
					execute();
				}

				virtual void execute() const override{
					auto reg = elang::vm::machine::register_manager.find("ecx");
					if (reg->read<unsigned int>() != 0u){//Do loop
						reg->dec<unsigned int>();//Decrement counter
						elang::vm::machine::cached_registers.instruction_pointer->write(operands_[0]->read<uint64_type>());//Jump
					}
				}
			};
		}
	}
}

#endif /* !ELANG_LOOP_INSTRUCTION_H */
