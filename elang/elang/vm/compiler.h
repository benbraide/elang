#pragma once

#ifndef ELANG_COMPILER_H
#define ELANG_COMPILER_H

#include "../asm/instruction_section.h"

#include "register_store.h"

namespace elang::vm{
	enum class compiler_error{
		nil,
		bad_char,
		number_too_small,
		number_too_big,
		unreachable,
	};

	enum class compiler_warning{
		nil,
		type_coercion
	};

	class compiler{
	public:
		typedef elang::easm::section_id section_id_type;
		typedef elang::easm::instruction_error error_type;

		typedef elang::easm::instruction_section_base section_type;
		typedef std::shared_ptr<section_type> section_ptr_type;

		typedef std::vector<machine_register *> register_list_type;
		typedef std::unordered_map<section_id_type, section_ptr_type> section_map_type;

		compiler();

		register_store &store();

		section_type &section(section_id_type id);

		void push_register(machine_register &reg);

		machine_register *pop_register();

		void reset_expression_type();

		void set_expression_type(machine_value_type_id left, machine_value_type_id right);

		machine_value_type_id get_expression_type() const;

		unsigned int label_count();

		void reset_warnings();

		void add_warning(compiler_warning value);

	private:
		register_store store_;
		section_map_type section_map_;
		register_list_type register_list_;
		machine_value_type_id expression_type_;
		unsigned int label_count_;
	};
}

#endif /* !ELANG_COMPILER_H */
