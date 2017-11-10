#pragma once

#ifndef ELANG_LITERAL_VALUE_AST_H
#define ELANG_LITERAL_VALUE_AST_H

#include <climits>

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

	static void check_char_count(elang::common::string_quote_type quote, const std::string &value){
		if (is_wide(quote)){
			std::wstring escaped_value;
			elang::common::utils::escape_string(value.begin(), value.end(), escaped_value);
			if (escaped_value.size() != 1u)
				throw elang::vm::compiler_error::bad_char;
		}
		else{//Narrow
			std::string escaped_value;
			elang::common::utils::escape_string(value.begin(), value.end(), escaped_value);
			if (escaped_value.size() != 1u)
				throw elang::vm::compiler_error::bad_char;
		}
	}

	template <typename target_type, typename value_type>
	static void check_numeric_size(value_type value){
		if (value < static_cast<value_type>(std::numeric_limits<target_type>::min()))
			throw elang::vm::compiler_error::number_too_small;

		if (static_cast<value_type>(std::numeric_limits<target_type>::max()) < value)
			throw elang::vm::compiler_error::number_too_big;
	}

	template <typename target_type>
	static void check_unsigned_integral_size(unsigned __int64 value){
		if (static_cast<unsigned __int64>(std::numeric_limits<target_type>::max()) < value)
			throw elang::vm::compiler_error::number_too_big;
	}

	void operator()(const integral_literal &ast) const{
		using instruction_operand_ptr_type = elang::easm::instruction::operand_base::ptr_type;

		elang::vm::machine_value_type_id value_type;
		switch (ast.second.value_or(elang::common::numeric_literal_suffix::int32)){
		case elang::common::numeric_literal_suffix::int8:
			check_numeric_size<__int8>(ast.first);
			value_type = elang::vm::machine_value_type_id::byte;
			break;
		case elang::common::numeric_literal_suffix::uint8:
			check_unsigned_integral_size<__int8>(ast.first);
			value_type = elang::vm::machine_value_type_id::byte;
			break;
		case elang::common::numeric_literal_suffix::int16:
			check_numeric_size<__int16>(ast.first);
			value_type = elang::vm::machine_value_type_id::word;
			break;
		case elang::common::numeric_literal_suffix::uint16:
			check_unsigned_integral_size<__int16>(ast.first);
			value_type = elang::vm::machine_value_type_id::word;
			break;
		case elang::common::numeric_literal_suffix::int32:
			check_numeric_size<__int32>(ast.first);
			value_type = elang::vm::machine_value_type_id::dword;
			break;
		case elang::common::numeric_literal_suffix::uint32:
			check_unsigned_integral_size<__int32>(ast.first);
			value_type = elang::vm::machine_value_type_id::dword;
			break;
		case elang::common::numeric_literal_suffix::int64:
			check_numeric_size<__int64>(ast.first);
			value_type = elang::vm::machine_value_type_id::qword;
			break;
		case elang::common::numeric_literal_suffix::uint64:
			check_unsigned_integral_size<__int64>(ast.first);
			value_type = elang::vm::machine_value_type_id::qword;
			break;
		default:
			throw elang::vm::compiler_error::unreachable;
			break;
		}

		auto reg = elang::vm::machine::compiler.store().get(value_type);
		if (reg == nullptr)//Error
			throw elang::vm::machine_error::no_register;

		auto reg_op = std::make_shared<elang::easm::instruction::register_operand>(*reg);
		auto const_op = std::make_shared<elang::easm::instruction::constant_value_operand<__int64>>(ast.first);
		auto instruction = std::make_shared<elang::easm::instruction::mov>(std::vector<instruction_operand_ptr_type>({ reg_op, const_op }));

		elang::vm::machine::compiler.section(elang::easm::section_id::text).add(instruction);
	}

	void operator()(const real_literal &ast) const{
		using instruction_operand_ptr_type = elang::easm::instruction::operand_base::ptr_type;

		auto reg = elang::vm::machine::compiler.store().get(elang::vm::machine_value_type_id::float_);
		if (reg == nullptr)//Error
			throw elang::vm::machine_error::no_register;

		auto reg_op = std::make_shared<elang::easm::instruction::register_operand>(*reg);
		auto const_op = std::make_shared<elang::easm::instruction::constant_value_operand<long double>>(ast.first);
		auto instruction = std::make_shared<elang::easm::instruction::mov>(std::vector<instruction_operand_ptr_type>({ reg_op, const_op }));

		elang::vm::machine::compiler.section(elang::easm::section_id::text).add(instruction);
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
			check_char_count(ast.first, value);

			auto value_type = (is_wide(ast.first) ? elang::vm::machine_value_type_id::word : elang::vm::machine_value_type_id::byte);
			auto reg = elang::vm::machine::compiler.store().get(value_type);
			if (reg == nullptr)//Error
				throw elang::vm::machine_error::no_register;

			auto reg_op = std::make_shared<elang::easm::instruction::register_operand>(*reg);
			auto str_op = std::make_shared<elang::easm::instruction::string_value_operand>(std::move(value));
			auto instruction = std::make_shared<elang::easm::instruction::mov>(std::vector<instruction_operand_ptr_type>({ reg_op, str_op }));

			elang::vm::machine::compiler.section(elang::easm::section_id::text).add(instruction);
			return;
		}

		value.append("\\0");
		instruction_ptr_type instruction;

		auto label = elang::vm::machine::compiler.generate_label(elang::vm::label_type::constant);
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
