#pragma once

#ifndef ELANG_CONSTANT_VALUE_PARSER_H
#define ELANG_CONSTANT_VALUE_PARSER_H

#include "../ast/constant_value_ast.h"

namespace elang::grammar::parser{
	namespace x3 = boost::spirit::x3;

	struct constant_value_symbols : x3::symbols<elang::common::constant_value>{
		constant_value_symbols(){
			add
				("true", elang::common::constant_value::true_)
				("false", elang::common::constant_value::false_)
				("#ind", elang::common::constant_value::indeterminate)
				("nullptr", elang::common::constant_value::nullptr_)
				("NaN", elang::common::constant_value::nan_)
				("undefined", elang::common::constant_value::undefined_)
				("#inf", elang::common::constant_value::infinite_)
				;
		}
	} constant_value_symbols_;

	x3::rule<class constant_value, ast::constant_value> const constant_value = "constant_value";

	auto const constant_value_def = utils::keyword(constant_value_symbols_);

	BOOST_SPIRIT_DEFINE(constant_value);
};

#endif /* !ELANG_CONSTANT_VALUE_PARSER_H */
