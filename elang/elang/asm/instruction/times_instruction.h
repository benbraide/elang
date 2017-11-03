#pragma once

#ifndef ELANG_TIMES_INSTRUCTION_H
#define ELANG_TIMES_INSTRUCTION_H

#include "instruction_base.h"

namespace elang{
	namespace easm{
		namespace instruction{
			class times : public base{
			public:
				times(unsigned int count, ptr_type value)
					: base("times", operand_ptr_list_type{}), count_(count), value_(value){}

				virtual size_type instruction_bytes() const override{
					return (count_ * value_->instruction_bytes());
				}

				virtual void execute_and_update_instruction_pointer(register_type &instruction_pointer) const override{}

				virtual void execute() const override{}

				virtual void print(writer_type &writer, writer_type &wide_writer) const override{
					writer << name_ << " " << count_ << " " << writer_type::manip_type::flush;
					value_->print(writer, wide_writer);
				}

				virtual unsigned int count() const{
					return count_;
				}

				virtual ptr_type value() const{
					return value_;
				}

			protected:
				unsigned int count_;
				ptr_type value_;
			};
		}
	}
}

#endif /* !ELANG_TIMES_INSTRUCTION_H */
