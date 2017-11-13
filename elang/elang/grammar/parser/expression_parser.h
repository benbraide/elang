#pragma once

#ifndef ELANG_EXPRESSION_PARSER_H
#define ELANG_EXPRESSION_PARSER_H

#include "type_parser.h"
#include "identifier_parser.h"
#include "literal_value_parser.h"
#include "constant_value_parser.h"
#include "operator_symbol_parser.h"

#include "../ast/expression_ast.h"

namespace elang::grammar::parser{
	namespace x3 = boost::spirit::x3;

	x3::rule<class static_cast_expression, ast::static_cast_expression> const static_cast_expression = "static_cast_expression";
	x3::rule<class reinterpret_cast_expression, ast::reinterpret_cast_expression> const reinterpret_cast_expression = "reinterpret_cast_expression";
	x3::rule<class dynamic_cast_expression, ast::dynamic_cast_expression> const dynamic_cast_expression = "dynamic_cast_expression";

	x3::rule<class cast_expression, ast::cast_expression> const cast_expression = "cast_expression";

	x3::rule<class non_operator_term, ast::non_operator_term> const non_operator_term = "non_operator_term";

	x3::rule<class member_access, ast::member_access> const member_access = "member_access";
	x3::rule<class call, ast::call> const call = "call";
	x3::rule<class index, ast::index> const index = "index";

	x3::rule<class right_unary_term, ast::right_unary_term> const right_unary_term = "right_unary_term";
	x3::rule<class left_unary_term, ast::left_unary_term> const left_unary_term = "left_unary_term";
	x3::rule<class term, ast::term> const term = "term";

	x3::rule<class factor, ast::factor> const factor = "factor";
	x3::rule<class factor_expression, ast::factor_expression> const factor_expression = "factor_expression";

	x3::rule<class addition, ast::addition> const addition = "addition";
	x3::rule<class addition_expression, ast::addition_expression> const addition_expression = "addition_expression";

	x3::rule<class shift, ast::shift> const shift = "shift";
	x3::rule<class shift_expression, ast::shift_expression> const shift_expression = "shift_expression";

	x3::rule<class relation, ast::relation> const relation = "relation";
	x3::rule<class relation_expression, ast::relation_expression> const relation_expression = "relation_expression";

	x3::rule<class equality, ast::equality> const equality = "equality";
	x3::rule<class equality_expression, ast::equality_expression> const equality_expression = "equality_expression";

	x3::rule<class bitwise_and, ast::bitwise_and> const bitwise_and = "bitwise_and";
	x3::rule<class bitwise_and_expression, ast::bitwise_and_expression> const bitwise_and_expression = "bitwise_and_expression";

	x3::rule<class bitwise_xor, ast::bitwise_xor> const bitwise_xor = "bitwise_xor";
	x3::rule<class bitwise_xor_expression, ast::bitwise_xor_expression> const bitwise_xor_expression = "bitwise_xor_expression";

	x3::rule<class bitwise_or, ast::bitwise_or> const bitwise_or = "bitwise_or";
	x3::rule<class bitwise_or_expression, ast::bitwise_or_expression> const bitwise_or_expression = "bitwise_or_expression";

	x3::rule<class logical_and, ast::logical_and> const logical_and = "logical_and";
	x3::rule<class logical_and_expression, ast::logical_and_expression> const logical_and_expression = "logical_and_expression";

	x3::rule<class logical_or, ast::logical_or> const logical_or = "logical_or";
	x3::rule<class logical_or_expression, ast::logical_or_expression> const logical_or_expression = "logical_or_expression";

	x3::rule<class right_associative, ast::right_associative> const right_associative = "right_associative";
	x3::rule<class right_associative_expression, ast::right_associative_expression> const right_associative_expression = "right_associative_expression";

	x3::rule<class ternary, ast::ternary> const ternary = "ternary";
	x3::rule<class ternary_expression, ast::ternary_expression> const ternary_expression = "ternary_expression";

	x3::rule<class comma, ast::comma> const comma = "comma";
	x3::rule<class comma_expression, ast::comma_expression> const comma_expression = "comma_expression";

	x3::rule<class expression, ast::expression> const expression = "expression";
	x3::rule<class non_comma_expression, ast::non_comma_expression> const non_comma_expression = "non_comma_expression";
	x3::rule<class grouped_expression, ast::grouped_expression> const grouped_expression = "grouped_expression";

	struct member_access_operator_symbols : x3::symbols<elang::common::operator_id>{
		member_access_operator_symbols(){
			using operator_id_type = elang::common::operator_id;

			add
				("->", operator_id_type::member_pointer_access)
				(".", operator_id_type::member_access)
				;
		}
	} member_access_operator_symbols_;

	struct right_unary_operator_symbols : x3::symbols<elang::common::operator_id>{
		right_unary_operator_symbols(){
			using operator_id_type = elang::common::operator_id;

			add
				("++", operator_id_type::increment)
				("--", operator_id_type::decrement)
				;
		}
	} right_unary_operator_symbols_;

	struct left_unary_operator_symbols : x3::symbols<elang::common::operator_id>{
		left_unary_operator_symbols(){
			using operator_id_type = elang::common::operator_id;

			add
				("++", operator_id_type::increment)
				("--", operator_id_type::decrement)
				("*", operator_id_type::times)
				("+", operator_id_type::plus)
				("-", operator_id_type::minus)
				("&", operator_id_type::bitwise_and)
				("~", operator_id_type::bitwise_inverse)
				("!", operator_id_type::relational_not)
				("new", operator_id_type::new_)
				("delete", operator_id_type::delete_)
				("sizeof", operator_id_type::sizeof_)
				;
		}
	} left_unary_operator_symbols_;

	struct factor_operator_symbols : x3::symbols<elang::common::operator_id>{
		factor_operator_symbols(){
			using operator_id_type = elang::common::operator_id;

			add
				("*", operator_id_type::times)
				("/", operator_id_type::divide)
				("%", operator_id_type::modulus)
				;
		}
	} factor_operator_symbols_;

	struct addition_operator_symbols : x3::symbols<elang::common::operator_id>{
		addition_operator_symbols(){
			using operator_id_type = elang::common::operator_id;

			add
				("+", operator_id_type::plus)
				("-", operator_id_type::minus)
				;
		}
	} addition_operator_symbols_;

	struct shift_operator_symbols : x3::symbols<elang::common::operator_id>{
		shift_operator_symbols(){
			using operator_id_type = elang::common::operator_id;

			add
				("<<", operator_id_type::left_shift)
				(">>", operator_id_type::right_shift)
				;
		}
	} shift_operator_symbols_;

	struct relation_operator_symbols : x3::symbols<elang::common::operator_id>{
		relation_operator_symbols(){
			using operator_id_type = elang::common::operator_id;

			add
				("<=", operator_id_type::less_or_equal)
				(">=", operator_id_type::greater_or_equal)
				("<", operator_id_type::less)
				(">", operator_id_type::greater)
				;
		}
	} relation_operator_symbols_;

	struct equality_operator_symbols : x3::symbols<elang::common::operator_id>{
		equality_operator_symbols(){
			using operator_id_type = elang::common::operator_id;

			add
				("==", operator_id_type::equality)
				("!=", operator_id_type::inverse_equality)
				;
		}
	} equality_operator_symbols_;

	struct bitwise_and_operator_symbols : x3::symbols<elang::common::operator_id>{
		bitwise_and_operator_symbols(){
			using operator_id_type = elang::common::operator_id;

			add
				("&", operator_id_type::bitwise_and)
				;
		}
	} bitwise_and_operator_symbols_;

	struct bitwise_xor_operator_symbols : x3::symbols<elang::common::operator_id>{
		bitwise_xor_operator_symbols(){
			using operator_id_type = elang::common::operator_id;

			add
				("^", operator_id_type::bitwise_xor)
				;
		}
	} bitwise_xor_operator_symbols_;

	struct bitwise_or_operator_symbols : x3::symbols<elang::common::operator_id>{
		bitwise_or_operator_symbols(){
			using operator_id_type = elang::common::operator_id;

			add
				("|", operator_id_type::bitwise_or)
				;
		}
	} bitwise_or_operator_symbols_;

	struct logical_and_operator_symbols : x3::symbols<elang::common::operator_id>{
		logical_and_operator_symbols(){
			using operator_id_type = elang::common::operator_id;

			add
				("&&", operator_id_type::relational_and)
				;
		}
	} logical_and_operator_symbols_;

	struct logical_or_operator_symbols : x3::symbols<elang::common::operator_id>{
		logical_or_operator_symbols(){
			using operator_id_type = elang::common::operator_id;

			add
				("||", operator_id_type::relational_or)
				;
		}
	} logical_or_operator_symbols_;

	struct assignment_operator_symbols : x3::symbols<elang::common::operator_id>{
		assignment_operator_symbols(){
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
				("=", operator_id_type::assignment)
				;
		}
	} assignment_operator_symbols_;

	struct comma_operator_symbols : x3::symbols<elang::common::operator_id>{
		comma_operator_symbols(){
			using operator_id_type = elang::common::operator_id;

			add
				(",", operator_id_type::comma)
				;
		}
	} comma_operator_symbols_;

	auto const static_cast_expression_def = (x3::lit("static_cast") >> '<' >> type >> '>' >> grouped_expression);
	auto const reinterpret_cast_expression_def = (x3::lit("reinterpret_cast") >> '<' >> type >> '>' >> grouped_expression);
	auto const dynamic_cast_expression_def = (x3::lit("dynamic_cast") >> '<' >> type >> '>' >> grouped_expression);

	auto const cast_expression_def = (static_cast_expression | reinterpret_cast_expression | dynamic_cast_expression);

	auto const non_operator_term_def = (grouped_expression | constant_value | literal_value | cast_expression | identifier_compatible);

	auto const member_access_def = (member_access_operator_symbols_ >> identifier_compatible);
	auto const call_def = ('(' >> -expression >> ')');
	auto const index_def = ('[' >> expression >> ']');

	auto const right_unary_term_def = (non_operator_term >> +(member_access | call | index | right_unary_operator_symbols_));
	auto const left_unary_term_def = (left_unary_operator_symbols_ >> (non_operator_term | right_unary_term));
	auto const term_def = (right_unary_term | non_operator_term | left_unary_term);

	auto const factor_def = (factor_operator_symbols_ >> term);
	auto const factor_expression_def = (term >> +factor);

	auto const addition_def = (factor | (addition_operator_symbols_ >> (factor_expression | term)));
	auto const addition_expression_def = (term >> +addition);

	auto const shift_def = (addition | (shift_operator_symbols_ >> (addition_expression | term)));
	auto const shift_expression_def = (term >> +shift);

	auto const relation_def = (shift | (relation_operator_symbols_ >> (shift_expression | term)));
	auto const relation_expression_def = (term >> +relation);

	auto const equality_def = (relation | (equality_operator_symbols_ >> (relation_expression | term)));
	auto const equality_expression_def = (term >> +equality);

	auto const bitwise_and_def = (equality | (bitwise_and_operator_symbols_ >> (equality_expression | term)));
	auto const bitwise_and_expression_def = (term >> +bitwise_and);

	auto const bitwise_xor_def = (bitwise_and | (bitwise_xor_operator_symbols_ >> (bitwise_and_expression | term)));
	auto const bitwise_xor_expression_def = (term >> +bitwise_xor);

	auto const bitwise_or_def = (bitwise_xor | (bitwise_or_operator_symbols_ >> (bitwise_xor_expression | term)));
	auto const bitwise_or_expression_def = (term >> +bitwise_or);

	auto const logical_and_def = (bitwise_or | (logical_and_operator_symbols_ >> (bitwise_or_expression | term)));
	auto const logical_and_expression_def = (term >> +logical_and);

	auto const logical_or_def = (logical_and | (logical_or_operator_symbols_ >> (logical_and_expression | term)));
	auto const logical_or_expression_def = (term >> +logical_or);

	auto const right_associative_def = (logical_or | (assignment_operator_symbols_ >> (logical_or_expression | ternary | right_associative | term)));
	auto const right_associative_expression_def = (term >> +right_associative);

	auto const ternary_def = (right_associative >> ('?' >> (comma_expression | term) >> ':' >> (right_associative_expression | term)));
	auto const ternary_expression_def = (term >> +ternary);

	auto const comma_def = (ternary | (comma_operator_symbols_ >> (ternary_expression | term)));
	auto const comma_expression_def = (term >> +comma);

	auto const expression_def = (comma_expression | term);
	auto const non_comma_expression_def = (ternary_expression | term);
	auto const grouped_expression_def = ('(' >> expression >> ')');

	BOOST_SPIRIT_DEFINE(
		static_cast_expression,
		reinterpret_cast_expression,
		dynamic_cast_expression,
		cast_expression,
		non_operator_term,
		member_access,
		call,
		index,
		right_unary_term,
		left_unary_term,
		term,
		factor,
		factor_expression,
		addition,
		addition_expression,
		shift,
		shift_expression,
		relation,
		relation_expression,
		equality,
		equality_expression,
		bitwise_and,
		bitwise_and_expression,
		bitwise_xor,
		bitwise_xor_expression,
		bitwise_or,
		bitwise_or_expression,
		logical_and,
		logical_and_expression,
		logical_or,
		logical_or_expression,
		right_associative,
		right_associative_expression,
		ternary,
		ternary_expression,
		comma,
		comma_expression,
		expression,
		non_comma_expression,
		grouped_expression
	)
}

#endif /* !ELANG_EXPRESSION_PARSER_H */
