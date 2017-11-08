#pragma once

#ifndef ELANG_LITERAL_VALUE_PARSER_H
#define ELANG_LITERAL_VALUE_PARSER_H

#include "../ast/literal_value_ast.h"

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

	auto const integral_literal_def = x3::lexeme[(("0x" >> long_long_hex) | ("0b" >> long_long_bin) | ("0" >> long_long_oct) | x3::long_long) >> -integral_literal_symbols_];
	auto const real_literal_def = x3::lexeme[(long_double_ >> -real_literal_symbols_)];
}

#endif /* !ELANG_LITERAL_VALUE_PARSER_H */