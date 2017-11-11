#pragma once

#ifndef ELANG_IDENTIFIER_PARSER_H
#define ELANG_IDENTIFIER_PARSER_H

#include "../ast/identifier_ast.h"

#include "keyword_parser.h"
#include "operator_symbol_parser.h"

namespace elang::grammar::parser{
	namespace x3 = boost::spirit::x3;

	x3::rule<class identifier, ast::identifier> const identifier = "identifier";
	x3::rule<class operator_identifier, ast::operator_identifier> const operator_identifier = "operator_identifier";

	x3::rule<class global_qualified_identifier, ast::global_qualified_identifier> const global_qualified_identifier = "global_qualified_identifier";
	x3::rule<class qualified_identifier, ast::qualified_identifier> const qualified_identifier = "qualified_identifier";

	x3::rule<class identifier_compatible, ast::identifier_compatible> const identifier_compatible = "identifier_compatible";
	x3::rule<class non_operator_identifier_compatible, ast::non_operator_identifier_compatible> const non_operator_identifier_compatible = "non_operator_identifier_compatible";

	auto const identifier_def = (!keyword >> elang_identifier);
	auto const operator_identifier_def = (utils::keyword("operator") >> (
		((x3::lit("(") > ")") >> x3::attr(elang::common::operator_id::call)) |
		((x3::lit("[") > "]") >> x3::attr(elang::common::operator_id::index)) |
		operator_symbol |
		non_operator_identifier_compatible
	));

	auto const global_qualified_identifier_def = ("::" >> (operator_identifier | identifier));
	auto const qualified_identifier_def = ((global_qualified_identifier | operator_identifier | identifier) >> "::" >> (operator_identifier | identifier));

	auto const identifier_compatible_def = (qualified_identifier_def | global_qualified_identifier | operator_identifier | identifier);
	auto const non_operator_identifier_compatible_def = (qualified_identifier_def | global_qualified_identifier | identifier);

	BOOST_SPIRIT_DEFINE(
		identifier,
		operator_identifier,
		global_qualified_identifier,
		qualified_identifier,
		identifier_compatible,
		non_operator_identifier_compatible
	)
}

#endif /* !ELANG_IDENTIFIER_PARSER_H */
