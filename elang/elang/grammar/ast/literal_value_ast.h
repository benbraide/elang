#pragma once

#ifndef ELANG_LITERAL_VALUE_AST_H
#define ELANG_LITERAL_VALUE_AST_H

#include "../../common/numeric_literal_suffix.h"
#include "../../common/string_quote_type.h"

#include "../../vm/machine.h"

#include "asm_ast.h"

#define ELANG_AST_NUMERIC_SUFFIX boost::optional<elang::common::numeric_literal_suffix>

ELANG_AST_BEGIN

ELANG_AST_DECLARE_PAIR(integral_literal, __int64, ELANG_AST_NUMERIC_SUFFIX)
ELANG_AST_DECLARE_PAIR(real_literal, long double, ELANG_AST_NUMERIC_SUFFIX)
ELANG_AST_DECLARE_SINGLE_VARIANT(numeric_literal, integral_literal, real_literal)

ELANG_AST_DECLARE_PAIR(string_literal, elang::common::string_quote_type, std::vector<char>)
ELANG_AST_DECLARE_SINGLE_VARIANT(literal_value, numeric_literal, string_literal)

struct literal_value_traverser{
	static bool is_escaped(elang::common::string_quote_type value){
		return (
			value == elang::common::string_quote_type::escaped_narrow ||
			value == elang::common::string_quote_type::escaped_narrow_char ||
			value == elang::common::string_quote_type::escaped_wide ||
			value == elang::common::string_quote_type::escaped_wide_char
			);
	}

	static bool is_wide(elang::common::string_quote_type value){
		return (
			value == elang::common::string_quote_type::wide ||
			value == elang::common::string_quote_type::wide_char ||
			value == elang::common::string_quote_type::escaped_wide ||
			value == elang::common::string_quote_type::escaped_wide_char
			);
	}

	static bool is_char(elang::common::string_quote_type value){
		return (
			value == elang::common::string_quote_type::narrow_char ||
			value == elang::common::string_quote_type::wide_char ||
			value == elang::common::string_quote_type::escaped_narrow_char ||
			value == elang::common::string_quote_type::escaped_wide_char
			);
	}

	void operator()(const string_literal &ast) const{
		using instruction_operand_ptr_type = elang::easm::instruction::operand_base::ptr_type;
		using instruction_ptr_type = elang::easm::instruction::base::ptr_type;

		auto reg = elang::vm::machine::compiler.store().get(elang::vm::machine_value_type_id::qword);
		if (reg == nullptr)//Error
			throw elang::vm::machine_error::no_register;

		std::string value(ast.second.data(), ast.second.size());
		if (!is_escaped(ast.first))
			elang::common::utils::disable_string_escape(value);

		if (is_char(ast.first)){
			std::string escaped_value;
			elang::common::utils::escape_string(value.begin(), value.end(), escaped_value);
			if (escaped_value.size() != 1u)
				throw elang::vm::compiler_error::bad_char;
		}
		else//Append null character
			value.append("\\0");

		instruction_ptr_type instruction;
		auto label = ("__LC" + std::to_string(elang::vm::machine::compiler.label_count()) + "__");

		auto str_op = std::make_shared<elang::easm::instruction::string_value_operand>(std::move(value));
		if (is_wide(ast.first))
			instruction = std::make_shared<elang::easm::instruction::dw>(std::vector<instruction_operand_ptr_type>({ str_op }));
		else//Byte
			instruction = std::make_shared<elang::easm::instruction::db>(std::vector<instruction_operand_ptr_type>({ str_op }));

		elang::vm::machine::compiler.section(elang::easm::section_id::rodata).add(nullptr, label);
		elang::vm::machine::compiler.section(elang::easm::section_id::rodata).add(instruction);

		auto reg_op = std::make_shared<elang::easm::instruction::register_operand>(*reg);
		auto label_op = std::make_shared<elang::easm::instruction::label_operand>(label, std::vector<std::string>());
		instruction = std::make_shared<elang::easm::instruction::mov>(std::vector<instruction_operand_ptr_type>({ reg_op, label_op }));

		elang::vm::machine::compiler.section(elang::easm::section_id::text).add(instruction);
	}
};

ELANG_AST_END

ELANG_AST_ADAPT_PAIR(integral_literal)
ELANG_AST_ADAPT_PAIR(real_literal)
ELANG_AST_ADAPT_SINGLE(numeric_literal)

ELANG_AST_ADAPT_PAIR(string_literal)
ELANG_AST_ADAPT_SINGLE(literal_value)

#endif /* !ELANG_LITERAL_VALUE_AST_H */
