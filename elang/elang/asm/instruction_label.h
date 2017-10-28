#pragma once

#ifndef ELANG_INSTRUCTION_LABEL_H
#define ELANG_INSTRUCTION_LABEL_H

#include "../../common/output_writer.h"

#include "instruction_error.h"

namespace elang{
	namespace easm{
		class instruction_label{
		public:
			typedef instruction_error error_type;
			typedef elang::common::output_writer writer_type;

			instruction_label(instruction_label *parent, const std::string &name);

			void create() const;

			instruction_label *parent() const;

			int nested_level() const;

			const std::string &qualified_name() const;

			instruction_label *find(const std::string &name, bool is_qualified) const;

			void print(writer_type &writer, writer_type &wide_writer) const;

		protected:
			instruction_label *parent_;
			std::string name_;
			std::string qualified_name_;
		};
	}
}

#endif /* !ELANG_INSTRUCTION_LABEL_H */
