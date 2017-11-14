#pragma once

#ifndef ELANG_ASM_TRANSLATION_H
#define ELANG_ASM_TRANSLATION_H

#include <list>
#include <vector>
#include <unordered_map>

#include "../asm/instruction_section.h"

namespace elang::easm::instruction{
	class label_operand;
}

namespace elang::vm{
	class asm_translation{
	public:
		typedef unsigned __int64 uint64_type;
		typedef unsigned int uint_type;

		typedef std::size_t size_type;
		typedef elang::common::output_writer writer_type;

		typedef elang::easm::instruction::base instruction_type;
		typedef instruction_type::ptr_type instruction_ptr_type;

		typedef elang::easm::section_id section_id_type;
		typedef elang::easm::instruction_error error_type;
		typedef elang::easm::instruction_label label_type;
		typedef elang::easm::instruction::label_operand label_operand_type;

		typedef elang::easm::instruction_section_base section_type;
		typedef std::shared_ptr<section_type> section_ptr_type;

		typedef elang::easm::instruction_section def_section_type;

		typedef std::list<label_operand_type *> label_operand_list_type;
		typedef std::vector<section_id_type> section_id_list_type;
		typedef std::unordered_map<section_id_type, section_ptr_type> section_map_type;

		asm_translation();

		void bundle();

		void start_label(const std::string &value);

		void stack_size(size_type value);

		void add(section_id_type id);

		void add(uint_type nested_level, const std::string &label);

		void add(label_type &label);

		void add(label_operand_type &label_op);

		void add(instruction_ptr_type instruction);

		virtual void add(const std::string &start_label);

		virtual void add(size_type stack_size);

		section_type *active_section() const;

		label_type *active_label() const;

		label_type *find(const std::string &first, const std::vector<std::string> &rest) const;

		uint64_type find(const label_type &label) const;

		instruction_type *find(uint64_type address) const;

		uint64_type find_address(const std::string &first, const std::vector<std::string> &rest) const;

		void print(writer_type &writer) const;

		const std::string &start_label() const;

		size_type stack_size() const;

	private:
		label_type *active_label_;
		section_type *active_section_;
		section_map_type section_map_;
		section_id_list_type section_order_;
		label_operand_list_type label_operand_list_;
		std::string start_label_;
		size_type stack_size_;
	};
}

#endif /* !ELANG_ASM_TRANSLATION_H */
