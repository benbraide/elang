#pragma once

#ifndef ELANG_OPERATOR_SYMBOL_PARSER_H
#define ELANG_OPERATOR_SYMBOL_PARSER_H

#include "../ast/operator_symbol_ast.h"

namespace elang::grammar::parser{
	namespace x3 = boost::spirit::x3;

	struct operator_symbols : x3::symbols<elang::common::operator_id>{
		operator_symbols(){
			using operator_id_type = elang::common::operator_id;

			add
				("<<=", operator_id_type::compound_left_shift)
				(">>=", operator_id_type::compound_right_shift)
				("*=", operator_id_type::compound_times)
				("/=", operator_id_type::compound_divide)
				("%=", operator_id_type::compound_modulus)
				("+=", operator_id_type::compound_plus)
				("-=", operator_id_type::compound_minus)
				("&=", operator_id_type::compound_bitwise_and)
				("^=", operator_id_type::compound_bitwise_xor)
				("|=", operator_id_type::compound_bitwise_or)
				("==", operator_id_type::equality)
				("!=", operator_id_type::inverse_equality)
				("&&", operator_id_type::relational_and)
				("||", operator_id_type::relational_or)
				("->", operator_id_type::member_pointer_access)
				("<<", operator_id_type::left_shift)
				(">>", operator_id_type::right_shift)
				("<=", operator_id_type::less_or_equal)
				(">=", operator_id_type::greater_or_equal)
				("++", operator_id_type::increment)
				("--", operator_id_type::decrement)
				("=", operator_id_type::assignment)
				(".", operator_id_type::member_access)
				("*", operator_id_type::times)
				("/", operator_id_type::divide)
				("%", operator_id_type::modulus)
				("+", operator_id_type::plus)
				("-", operator_id_type::minus)
				("<", operator_id_type::less)
				(">", operator_id_type::greater)
				("&", operator_id_type::bitwise_and)
				("^", operator_id_type::bitwise_xor)
				("|", operator_id_type::bitwise_or)
				("~", operator_id_type::bitwise_inverse)
				("!", operator_id_type::relational_not)
				(",", operator_id_type::comma)
				("new", operator_id_type::new_)
				("delete", operator_id_type::delete_)
				("sizeof", operator_id_type::sizeof_)
				;
		}
	} operator_symbols_;

	x3::rule<class operator_symbol, ast::operator_symbol> const operator_symbol = "operator_symbol";

	auto const operator_symbol_def = operator_symbols_;

	BOOST_SPIRIT_DEFINE(operator_symbol)
}

#endif /* !ELANG_OPERATOR_SYMBOL_PARSER_H */
