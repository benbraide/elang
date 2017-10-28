#pragma once

#ifndef ELANG_INSTRUCTION_SECTION_H
#define ELANG_INSTRUCTION_SECTION_H

#include "../../common/output_writer.h"

#include "instruction_error.h"

namespace elang{
	namespace easm{
		enum class section_id{
			rodata,
			data,
			text,
		};

		class instruction_section{
		public:
			typedef section_id id_type;
			typedef instruction_error error_type;

			typedef elang::common::output_writer writer_type;

			explicit instruction_section(id_type id);

			id_type id() const;

			void create() const;

			void print(writer_type &writer, writer_type &wide_writer) const;

		protected:
			id_type id_;
		};
	}
}

#endif /* !ELANG_INSTRUCTION_SECTION_H */
