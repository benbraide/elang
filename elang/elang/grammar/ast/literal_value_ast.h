#pragma once

#ifndef ELANG_LITERAL_VALUE_AST_H
#define ELANG_LITERAL_VALUE_AST_H

#include <climits>

#include "ast_common.h"

#define ELANG_AST_NUMERIC_SUFFIX boost::optional<elang::common::numeric_literal_suffix>

ELANG_AST_BEGIN

using instruction_operand_ptr_type = elang::easm::instruction::operand_base::ptr_type;

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

	template <typename string_type>
	static void get_char(const std::string &value, bool is_escaped, operand_value_info &out){
		if (is_escaped){
			string_type escaped_value;
			elang::common::utils::escape_string(value.begin(), value.end(), escaped_value);
			if (escaped_value.size() == 1u)
				out.value = static_cast<__int64>(escaped_value[0]);
			else//Error
				throw elang::vm::compiler_error::bad_char;
		}
		else if (value.size() == 1u)
			out.value = static_cast<__int64>(value[0]);
		else//Error
			throw elang::vm::compiler_error::bad_char;

		if (std::is_same<string_type, std::string>::value)
			out.type = elang::vm::machine::compiler.find_primitive_type(elang::common::primitive_type_id::char_);
		else//Wide
			out.type = elang::vm::machine::compiler.find_primitive_type(elang::common::primitive_type_id::wchar_);
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

	ELANG_AST_COMMON_TRAVERSER_BEGIN(literal_value_traverser)

	void operator()(const integral_literal &ast) const{
		auto suffix = ast.second.value_or(elang::common::numeric_literal_suffix::int32);
		switch (suffix){
		case elang::common::numeric_literal_suffix::int8:
			check_numeric_size<__int8>(ast.first);
			ELANG_AST_COMMON_TRAVERSER_OUT->type = elang::vm::machine::compiler.find_primitive_type(elang::common::primitive_type_id::int8_);
			break;
		case elang::common::numeric_literal_suffix::uint8:
			check_unsigned_integral_size<unsigned __int8>(ast.first);
			ELANG_AST_COMMON_TRAVERSER_OUT->type = elang::vm::machine::compiler.find_primitive_type(elang::common::primitive_type_id::uint8_);
			break;
		case elang::common::numeric_literal_suffix::int16:
			check_numeric_size<__int16>(ast.first);
			ELANG_AST_COMMON_TRAVERSER_OUT->type = elang::vm::machine::compiler.find_primitive_type(elang::common::primitive_type_id::int16_);
			break;
		case elang::common::numeric_literal_suffix::uint16:
			check_unsigned_integral_size<unsigned __int16>(ast.first);
			ELANG_AST_COMMON_TRAVERSER_OUT->type = elang::vm::machine::compiler.find_primitive_type(elang::common::primitive_type_id::uint16_);
			break;
		case elang::common::numeric_literal_suffix::int32:
			check_numeric_size<__int32>(ast.first);
			ELANG_AST_COMMON_TRAVERSER_OUT->type = elang::vm::machine::compiler.find_primitive_type(elang::common::primitive_type_id::int32_);
			break;
		case elang::common::numeric_literal_suffix::uint32:
			check_unsigned_integral_size<unsigned __int32>(ast.first);
			ELANG_AST_COMMON_TRAVERSER_OUT->type = elang::vm::machine::compiler.find_primitive_type(elang::common::primitive_type_id::uint32_);
			break;
		case elang::common::numeric_literal_suffix::int64:
			check_numeric_size<__int64>(ast.first);
			ELANG_AST_COMMON_TRAVERSER_OUT->type = elang::vm::machine::compiler.find_primitive_type(elang::common::primitive_type_id::int64_);
			break;
		case elang::common::numeric_literal_suffix::uint64:
			check_unsigned_integral_size<unsigned __int64>(ast.first);
			ELANG_AST_COMMON_TRAVERSER_OUT->type = elang::vm::machine::compiler.find_primitive_type(elang::common::primitive_type_id::uint64_);
			break;
		default:
			throw elang::vm::compiler_error::unreachable;
			break;
		}

		ELANG_AST_COMMON_TRAVERSER_OUT->value = ast.first;
		ELANG_AST_COMMON_TRAVERSER_OUT->is_constant = true;
		ELANG_AST_COMMON_TRAVERSER_OUT->is_static = true;
	}

	void operator()(const real_literal &ast) const{
		ELANG_AST_COMMON_TRAVERSER_OUT->type = elang::vm::machine::compiler.find_primitive_type(elang::common::primitive_type_id::float_);
		ELANG_AST_COMMON_TRAVERSER_OUT->value = ast.first;
		ELANG_AST_COMMON_TRAVERSER_OUT->is_constant = true;
		ELANG_AST_COMMON_TRAVERSER_OUT->is_static = true;
	}

	void operator()(const string_literal &ast) const{
		using instruction_operand_ptr_type = elang::easm::instruction::operand_base::ptr_type;
		using instruction_ptr_type = elang::easm::instruction::base::ptr_type;

		auto reg = elang::vm::machine::compiler.store().get(elang::vm::machine_value_type_id::qword);
		if (reg == nullptr)//Error
			throw elang::vm::machine_error::no_register;

		std::string value(ast.second.data(), ast.second.size());
		auto is_wide = literal_value_traverser::is_wide(ast.first);

		if (!is_char(ast.first)){//String
			if (!is_escaped(ast.first))
				elang::common::utils::disable_string_escape(value);

			value.append("\\0");
			ELANG_AST_COMMON_TRAVERSER_OUT->value = string_operand_value_info{
				std::move(value),
				is_wide
			};

			auto str_op = std::make_shared<elang::easm::instruction::string_value_operand>(std::move(value));
			auto label = elang::vm::machine::compiler.generate_label(elang::vm::label_type::constant);

			instruction_ptr_type instruction;
			if (is_wide)
				instruction = std::make_shared<elang::easm::instruction::dw>(std::vector<instruction_operand_ptr_type>({ str_op }));
			else//Byte
				instruction = std::make_shared<elang::easm::instruction::db>(std::vector<instruction_operand_ptr_type>({ str_op }));

			elang::vm::machine::compiler.section(elang::easm::section_id::rodata).add(nullptr, label);
			elang::vm::machine::compiler.section(elang::easm::section_id::rodata).add(instruction);
		}
		else if (is_wide)//Wide char
			get_char<std::wstring>(value, is_escaped(ast.first), ELANG_AST_COMMON_TRAVERSER_OUT_DREF);
		else//Narrow char
			get_char<std::string>(value, is_escaped(ast.first), ELANG_AST_COMMON_TRAVERSER_OUT_DREF);

		ELANG_AST_COMMON_TRAVERSER_OUT->is_constant = true;
		ELANG_AST_COMMON_TRAVERSER_OUT->is_static = true;
	}

	void operator()(const numeric_literal &ast) const{
		boost::apply_visitor(literal_value_traverser(ELANG_AST_COMMON_TRAVERSER_OUT_DREF), ast.value);
	}

	void operator()(const literal_value &ast) const{
		boost::apply_visitor(literal_value_traverser(ELANG_AST_COMMON_TRAVERSER_OUT_DREF), ast.value);
	}
};

ELANG_AST_END

ELANG_AST_ADAPT_PAIR(integral_literal)
ELANG_AST_ADAPT_PAIR(real_literal)
ELANG_AST_ADAPT_SINGLE(numeric_literal)

ELANG_AST_ADAPT_PAIR(string_literal)
ELANG_AST_ADAPT_SINGLE(literal_value)

#endif /* !ELANG_LITERAL_VALUE_AST_H */
