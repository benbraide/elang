#pragma once

#ifndef ELANG_LITERAL_VALUE_PARSER_H
#define ELANG_LITERAL_VALUE_PARSER_H

#include "../ast/literal_value_ast.h"

#define ELANG_PARSER_STRING_NAME(n) ELANG_AST_JOIN(n, _string_literal)
#define ELANG_PARSER_ESC_STRING_NAME(n) ELANG_AST_JOIN(escaped_, ELANG_PARSER_STRING_NAME(n))

#define ELANG_PARSER_STRING_DEF_NAME(n) ELANG_AST_JOIN(ELANG_PARSER_STRING_NAME(n), _def)
#define ELANG_PARSER_ESC_STRING_DEF_NAME(n) ELANG_AST_JOIN(ELANG_PARSER_ESC_STRING_NAME(n), _def)

#define ELANG_PARSER_DEFINE_STRING(n, q)\
x3::rule<class ELANG_PARSER_STRING_NAME(n), ast::string_literal> const ELANG_PARSER_STRING_NAME(n) = #n "_string_literal";\
x3::rule<class ELANG_PARSER_ESC_STRING_NAME(n), ast::string_literal> const ELANG_PARSER_ESC_STRING_NAME(n) = "escaped_" #n "_string_literal";\
\
auto const ELANG_PARSER_STRING_DEF_NAME(n) = ("@" q >> x3::attr(elang::common::string_quote_type::n) >> x3::lexeme[*(~x3::char_(q))] >> q);\
auto const ELANG_PARSER_ESC_STRING_DEF_NAME(n) = (q >> x3::attr(elang::common::string_quote_type::ELANG_AST_JOIN(escaped_, n)) >> x3::lexeme[*((x3::char_('\\') >> q) | ~x3::char_(q))] >> q);\
\
BOOST_SPIRIT_DEFINE(ELANG_PARSER_STRING_NAME(n), ELANG_PARSER_ESC_STRING_NAME(n))

namespace elang::grammar::parser{
	namespace x3 = boost::spirit::x3;

	struct integral_literal_symbols : x3::symbols<elang::common::numeric_literal_suffix>{
		integral_literal_symbols(){
			add
				("i8", elang::common::numeric_literal_suffix::int8)
				("ui8", elang::common::numeric_literal_suffix::uint8)
				("i16", elang::common::numeric_literal_suffix::int16)
				("ui16", elang::common::numeric_literal_suffix::uint16)
				("i32", elang::common::numeric_literal_suffix::int32)
				("ui32", elang::common::numeric_literal_suffix::uint32)
				("i64", elang::common::numeric_literal_suffix::int64)
				("ui64", elang::common::numeric_literal_suffix::uint64)
				("u", elang::common::numeric_literal_suffix::uint32)
				;
		}
	} integral_literal_symbols_;

	struct real_literal_symbols : x3::symbols<elang::common::numeric_literal_suffix>{
		real_literal_symbols(){
			add
				("f", elang::common::numeric_literal_suffix::float_)
				("l", elang::common::numeric_literal_suffix::long_double_)
				;
		}
	} real_literal_symbols_;

	hex_parser<__int64> const long_long_hex = {};
	oct_parser<__int64> const long_long_oct = {};
	bin_parser<__int64> const long_long_bin = {};

	x3::rule<class integral_literal, ast::integral_literal> const integral_literal = "integral_literal";
	x3::rule<class real_literal, ast::real_literal> const real_literal = "real_literal";
	x3::rule<class numeric_literal, ast::numeric_literal> const numeric_literal = "numeric_literal";

	x3::rule<class literal_value, ast::literal_value> const literal_value = "literal_value";

	auto const integral_literal_def = x3::lexeme[(("0x" >> long_long_hex) | ("0b" >> long_long_bin) | ("0" >> long_long_oct) | x3::long_long) >> -integral_literal_symbols_];
	auto const real_literal_def = x3::lexeme[(long_double_ >> -real_literal_symbols_)];
	auto const numeric_literal_def = (real_literal | integral_literal);

	ELANG_PARSER_DEFINE_STRING(narrow, "\"");
	ELANG_PARSER_DEFINE_STRING(wide, "L\"");

	ELANG_PARSER_DEFINE_STRING(narrow_char, "\'");
	ELANG_PARSER_DEFINE_STRING(wide_char, "L\'");

	auto const literal_value_def = (
		ELANG_PARSER_STRING_NAME(narrow) |
		ELANG_PARSER_ESC_STRING_NAME(narrow) |
		ELANG_PARSER_STRING_NAME(wide) |
		ELANG_PARSER_ESC_STRING_NAME(wide) |
		ELANG_PARSER_STRING_NAME(narrow_char) |
		ELANG_PARSER_ESC_STRING_NAME(narrow_char) |
		ELANG_PARSER_STRING_NAME(wide_char) |
		ELANG_PARSER_ESC_STRING_NAME(wide_char) |
		numeric_literal
	);

	BOOST_SPIRIT_DEFINE(
		integral_literal,
		real_literal,
		numeric_literal,
		literal_value
	)
}

#endif /* !ELANG_LITERAL_VALUE_PARSER_H */
