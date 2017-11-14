#pragma once

#ifndef ELANG_INSTRUCTION_LABEL_H
#define ELANG_INSTRUCTION_LABEL_H

#include <list>
#include <vector>

#include "../../common/output_writer.h"

#include "instruction_error.h"

namespace elang::easm{
	class instruction_label{
	public:
		typedef instruction_error error_type;
		typedef elang::common::output_writer writer_type;

		typedef std::shared_ptr<instruction_label> ptr_type;
		typedef std::list<instruction_label *> list_type;

		typedef std::vector<std::string> string_list_type;
		typedef string_list_type::const_iterator string_list_iterator_type;

		instruction_label(instruction_label *parent, const std::string &name);

		instruction_label *add(const std::string &label);

		instruction_label *parent(unsigned int index = 0u) const;

		unsigned int nested_level() const;

		instruction_label *find(const std::string &first, const std::vector<std::string> &rest) const;

		void print(writer_type &writer) const;

	protected:
		instruction_label *find_(string_list_iterator_type iter, string_list_iterator_type end) const;

		instruction_label *parent_;
		std::string name_;
		list_type list_;
	};
}

#endif /* !ELANG_INSTRUCTION_LABEL_H */
