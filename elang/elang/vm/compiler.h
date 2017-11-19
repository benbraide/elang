#pragma once

#ifndef ELANG_COMPILER_H
#define ELANG_COMPILER_H

#include "../common/constant_value.h"

#include "../asm/instruction_section.h"

#include "register_store.h"
#include "symbols_table.h"

namespace elang::vm{
	enum class compiler_error{
		nil,
		bad_char,
		number_too_small,
		number_too_big,
		ambiguous_function,
		redefinition,
		multiple_base,
		undefined,
		undefined_type,
		unique_return_type,
		type_expected,
		storage_expected,
		variable_expected,
		invalid_operation,
		invalid_cast,
		unreachable,
	};

	enum class compiler_warning{
		nil,
		type_coercion
	};

	enum class label_type{
		constant,
	};

	enum class compiler_state : unsigned int{
		nil					= (0 << 0x0000),
		compiling			= (1 << 0x0000),
		short_circuit		= (1 << 0x0001),
	};

	enum class mangle_target{
		pointer,
		array_,
		function_,
		variadic,
		const_,
		ref_,
		vref,
		operator_,
		constructor,
		destructor,
	};

	class compiler{
	public:
		typedef compiler_state state_type;

		typedef type_info::ptr_type type_info_ptr_type;
		typedef elang::common::primitive_type_id primitive_type_id_type;

		typedef elang::easm::section_id section_id_type;
		typedef elang::easm::instruction_error error_type;

		typedef elang::easm::instruction::operand_base::ptr_type instruction_operand_ptr_type;
		typedef elang::easm::instruction_section_base section_type;
		typedef std::shared_ptr<section_type> section_ptr_type;

		typedef std::unordered_map<section_id_type, section_ptr_type> section_map_type;
		typedef std::unordered_map<elang::common::primitive_type_id, type_info_ptr_type> primitive_type_map_type;

		struct current_context_info_type{
			storage_symbol_entry *value;
			bool bubble_search;
		};

		struct info_type{
			std::shared_ptr<namespace_symbol_entry> global_context;
			current_context_info_type current_context;
		};

		compiler();

		void boot();

		void begin_compilation();

		void end_compilation();

		bool is_compiling() const;

		void set_short_circuit(bool enabled = true);

		register_store &store();

		section_type &section(section_id_type id);

		std::string generate_label(label_type type);

		std::string mangle(mangle_target target) const;

		void reset_warnings();

		void add_warning(compiler_warning value);

		symbol_entry *find(const std::string &key) const;

		type_info_ptr_type find_primitive_type(primitive_type_id_type id) const;

		info_type &info();

	private:
		state_type states_;
		register_store store_;
		section_map_type section_map_;
		primitive_type_map_type primitive_type_map_;
		unsigned int label_count_;
		unsigned int short_circuit_count_;
		info_type info_;
	};

	ELANG_MAKE_OPERATORS(compiler_state);
}

#endif /* !ELANG_COMPILER_H */
