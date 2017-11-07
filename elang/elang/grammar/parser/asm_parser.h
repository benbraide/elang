#pragma once

#ifndef ELANG_ASM_PARSER_H
#define ELANG_ASM_PARSER_H

#include "../ast/asm_ast.h"

namespace elang::grammar::parser{
	namespace ast = elang::grammar::ast;
	namespace x3 = boost::spirit::x3;

	using instruction_operand_ptr_type = ast::instruction_operand_ptr_type;
	using instruction_ptr_type = ast::instruction_ptr_type;

	struct asm_section_symbols : x3::symbols<elang::easm::section_id>{
		asm_section_symbols(){
			add
				(".rodata", elang::easm::section_id::rodata)
				(".data", elang::easm::section_id::data)
				(".text", elang::easm::section_id::text)
				(".type", elang::easm::section_id::type)
				(".meta", elang::easm::section_id::meta)
				;
		}
	} asm_section_symbols_;

	struct asm_type_symbols : x3::symbols<elang::vm::machine_value_type_id>{
		asm_type_symbols(){
			add
				("byte", elang::vm::machine_value_type_id::byte)
				("dword", elang::vm::machine_value_type_id::dword)
				("qword", elang::vm::machine_value_type_id::qword)
				("word", elang::vm::machine_value_type_id::word)
				("float", elang::vm::machine_value_type_id::float_)
				;
		}
	} asm_type_symbols_;

	struct asm_mnemonic_symbols : x3::symbols<elang::easm::instruction::id>{
		asm_mnemonic_symbols(){
			add
				("nop", elang::easm::instruction::id::nop)
				("ret", elang::easm::instruction::id::ret)
				("push", elang::easm::instruction::id::push)
				("pop", elang::easm::instruction::id::pop)
				("syscall", elang::easm::instruction::id::int_)
				("call", elang::easm::instruction::id::call)
				("jmp", elang::easm::instruction::id::jmp)
				("jz", elang::easm::instruction::id::jz)
				("jnz", elang::easm::instruction::id::jnz)
				("je", elang::easm::instruction::id::je)
				("jne", elang::easm::instruction::id::jne)
				("jle", elang::easm::instruction::id::jle)
				("jl", elang::easm::instruction::id::jl)
				("jnle", elang::easm::instruction::id::jnle)
				("jnl", elang::easm::instruction::id::jnl)
				("jge", elang::easm::instruction::id::jge)
				("jg", elang::easm::instruction::id::jg)
				("jnge", elang::easm::instruction::id::jnge)
				("jng", elang::easm::instruction::id::jng)
				("loop", elang::easm::instruction::id::loop)
				("inc", elang::easm::instruction::id::inc)
				("dec", elang::easm::instruction::id::dec)
				("not", elang::easm::instruction::id::not)
				("lea", elang::easm::instruction::id::lea)
				("mov", elang::easm::instruction::id::mov)
				("add", elang::easm::instruction::id::add)
				("sub", elang::easm::instruction::id::sub)
				("mult", elang::easm::instruction::id::mult)
				("div", elang::easm::instruction::id::div)
				("mod", elang::easm::instruction::id::mod)
				("and", elang::easm::instruction::id::and_)
				("xor", elang::easm::instruction::id::xor_)
				("or", elang::easm::instruction::id::or_)
				("sal", elang::easm::instruction::id::sal)
				("sar", elang::easm::instruction::id::sar)
				("test", elang::easm::instruction::id::test)
				("cmp", elang::easm::instruction::id::cmp)
				("db", elang::easm::instruction::id::db)
				("dw", elang::easm::instruction::id::dw)
				("dd", elang::easm::instruction::id::dd)
				("dq", elang::easm::instruction::id::dq)
				("df", elang::easm::instruction::id::df)
				;
		}
	} asm_mnemonic_symbols_;

	struct asm_operator_symbols : x3::symbols<elang::easm::instruction_operator_id>{
		asm_operator_symbols(){
			add
				("+", elang::easm::instruction_operator_id::add)
				("-", elang::easm::instruction_operator_id::sub)
				("*", elang::easm::instruction_operator_id::mult)
				("/", elang::easm::instruction_operator_id::div)
				("%", elang::easm::instruction_operator_id::mod)
				("&", elang::easm::instruction_operator_id::and_)
				("|", elang::easm::instruction_operator_id::xor_)
				("^", elang::easm::instruction_operator_id::or_)
				("<<", elang::easm::instruction_operator_id::sal)
				(">>", elang::easm::instruction_operator_id::sar)
				;
		}
	} asm_operator_symbols_;

	x3::rule<class asm_integral_value, ast::asm_integral_value> const asm_integral_value = "asm_integral_value";
	x3::rule<class asm_float_value, ast::asm_float_value> const asm_float_value = "asm_float_value";

	x3::rule<class asm_string, ast::asm_string> const asm_string = "asm_string";
	x3::rule<class asm_identifier, ast::asm_identifier> const asm_identifier = "asm_identifier";
	x3::rule<class asm_absolute_identifier, ast::asm_absolute_identifier> const asm_absolute_identifier = "asm_absolute_identifier";

	x3::rule<class asm_section, ast::asm_section> const asm_section = "asm_section";
	x3::rule<class asm_label, ast::asm_label> const asm_label = "asm_label";

	x3::rule<class asm_expression, instruction_operand_ptr_type> const asm_expression = "asm_expression";
	x3::rule<class asm_grouped_expression, ast::asm_grouped_expression> const asm_grouped_expression = "asm_grouped_expression";

	x3::rule<class asm_memory, ast::asm_memory> const asm_memory = "asm_memory";
	x3::rule<class asm_sized_memory, ast::asm_sized_memory> const asm_sized_memory = "asm_sized_memory";

	x3::rule<class asm_expression_operand, ast::asm_expression_operand> const asm_expression_operand = "asm_expression_operand";
	x3::rule<class asm_operand, ast::asm_operand> const asm_operand = "asm_operand";
	x3::rule<class asm_typed_operand, ast::asm_typed_operand> const asm_typed_operand = "asm_typed_operand";

	x3::rule<class asm_struct_def_value, ast::asm_struct_def_value> const asm_struct_def_value = "asm_struct_def_value";
	x3::rule<class asm_struct_def, ast::asm_struct_def> const asm_struct_def = "asm_struct_def";
	x3::rule<class asm_type_def, ast::asm_type_def> const asm_type_def = "asm_type_def";

	x3::rule<class asm_times_instruction, ast::asm_times_instruction> const asm_times_instruction = "asm_times_instruction";
	x3::rule<class asm_instruction, ast::asm_instruction> const asm_instruction = "asm_instruction";
	x3::rule<class asm_extended_instruction, ast::asm_extended_instruction> const asm_extended_instruction = "asm_extended_instruction";

	x3::rule<class asm_instruction_set, ast::asm_instruction_set> const asm_instruction_set = "asm_instruction_set";
	x3::rule<class asm_instruction_set_value, ast::asm_instruction_set_value> const asm_instruction_set_value = "asm_instruction_set_value";

	x3::rule<class asm_skip> const asm_skip = "asm_skip";

	auto asm_parsed_single = [](auto &ctx){
		x3::_val(ctx) = ast::asm_traverser::operand(x3::_attr(ctx));
	};

	auto asm_parsed_expression = [](auto &ctx){
		x3::_val(ctx) = std::make_shared<elang::easm::instruction::expression_operand>(
			boost::fusion::at<boost::mpl::int_<0>>(x3::_attr(ctx)),
			x3::_val(ctx),
			ast::asm_traverser::operand(boost::fusion::at<boost::mpl::int_<1>>(x3::_attr(ctx)))
		);
	};

	signed_hex_parser<__int64> const asm_long_long_hex = {};
	signed_bin_parser<__int64> const asm_long_long_bin = {};

	auto const asm_integral_value_def = (x3::lexeme["0x" >> asm_long_long_hex] | x3::lexeme[asm_long_long_hex >> 'h'] | x3::lexeme[asm_long_long_bin >> 'b'] | x3::long_long);
	auto const asm_float_value_def = long_double_;

	auto const asm_string_def = ("'" >> x3::lexeme[*(~x3::char_("'"))] >> "'");
	auto const asm_identifier_def = (!x3::no_case[utils::keyword("section")] >> elang_identifier);
	auto const asm_absolute_identifier_def = (asm_identifier >> +('.' >> asm_identifier));

	auto const asm_section_def = x3::no_case[(utils::keyword("section") >> asm_section_symbols_)];
	auto const asm_label_def = (x3::lexeme[*x3::char_('.') >> asm_identifier >> utils::keyword(":")]);

	auto const asm_expression_def = (asm_expression_operand)[asm_parsed_single] >> *(asm_operator_symbols_ >> asm_expression_operand)[asm_parsed_expression];
	auto const asm_grouped_expression_def = ('(' >> asm_expression >> ')');

	auto const asm_memory_def = ('[' >> asm_expression >> ']');
	auto const asm_sized_memory_def = (asm_memory >> (asm_absolute_identifier | asm_identifier));

	auto const asm_expression_operand_def = (asm_grouped_expression | asm_float_value | asm_integral_value | asm_absolute_identifier | asm_identifier);

	auto const asm_operand_def = (
		asm_sized_memory |
		asm_memory |
		asm_string |
		asm_expression
	);

	auto const asm_typed_operand_def = (x3::no_case[asm_type_symbols_] >> asm_operand);

	auto const asm_struct_def_value_def = (asm_type_def >> x3::omit[x3::eol]);
	auto const asm_struct_def_def = (x3::no_case[utils::keyword("struct")] >> '{' >> x3::omit[x3::eol] >> +asm_struct_def_value >> '}');
	auto const asm_type_def_def = (asm_identifier >> (asm_struct_def | asm_type_symbols_));

	auto const asm_times_instruction_def = (x3::no_case[utils::keyword("times")] >> x3::uint_ >> (asm_extended_instruction | asm_instruction));
	auto const asm_instruction_def = (utils::keyword(x3::no_case[asm_mnemonic_symbols_]) >> -((asm_typed_operand | asm_operand) % ","));
	auto const asm_extended_instruction_def = (x3::lexeme['%' >> utils::keyword(x3::no_case[asm_mnemonic_symbols_])] >> -((asm_typed_operand | asm_operand) % ","));

	auto const asm_instruction_set_value_def = ((asm_section | asm_label | asm_times_instruction | asm_instruction | asm_type_def) >> x3::omit[(x3::eol | x3::eoi)]);
	auto const asm_instruction_set_def = *(asm_instruction_set_value);

	auto const asm_skip_def = ((x3::space - x3::eol) | (';' >> *x3::omit[(x3::char_ - x3::eol)]));

	BOOST_SPIRIT_DEFINE(
		asm_integral_value,
		asm_float_value,
		asm_string,
		asm_identifier,
		asm_absolute_identifier,
		asm_section,
		asm_label,
		asm_expression,
		asm_grouped_expression,
		asm_memory,
		asm_sized_memory,
		asm_expression_operand,
		asm_operand,
		asm_typed_operand,
		asm_struct_def_value,
		asm_struct_def,
		asm_type_def,
		asm_times_instruction,
		asm_instruction,
		asm_extended_instruction,
		asm_instruction_set_value,
		asm_instruction_set,
		asm_skip
	)
}

#endif /* !ELANG_ASM_PARSER_H */
