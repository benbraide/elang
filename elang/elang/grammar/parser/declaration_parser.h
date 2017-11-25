#pragma once

#ifndef ELANG_DECLARATION_PARSER_H
#define ELANG_DECLARATION_PARSER_H

#include "../ast/declaration_ast.h"

#include "type_parser.h"
#include "expression_parser.h"

namespace elang::grammar::parser{
	namespace x3 = boost::spirit::x3;

	struct storage_class_symbols : x3::symbols<elang::vm::symbol_entry_attribute>{
		storage_class_symbols(){
			add
				("static", elang::vm::symbol_entry_attribute::static_)
				("thread_local", elang::vm::symbol_entry_attribute::tls)
				;
		}
	} storage_class_symbols_;

	x3::rule<class variable_definition, ast::variable_definition> const variable_definition = "variable_definition";

	x3::rule<class variable_declaration, ast::variable_declaration> const variable_declaration = "variable_declaration";
	x3::rule<class multiple_variable_declaration, ast::multiple_variable_declaration> const multiple_variable_declaration = "multiple_variable_declaration";

	x3::rule<class variable_declaration_list, ast::variable_declaration_list> const variable_declaration_list = "variable_declaration_list";
	x3::rule<class parameter_variable_declaration_list, ast::parameter_variable_declaration_list> const parameter_variable_declaration_list = "parameter_variable_declaration_list";

	auto const variable_definition_def = (*storage_class_symbols_ >> type >> non_operator_identifier_compatible >> -('=' >> expression));

	auto const variable_declaration_def = (*storage_class_symbols_ >> type >> identifier >> -('=' >> non_comma_expression));
	auto const multiple_variable_declaration_def = (*x3::char_('.') >> -('&' >> x3::attr(true)) >> identifier >> -('=' >> non_comma_expression));

	auto const variable_declaration_list_def = (variable_declaration >> *("," >> multiple_variable_declaration));
	auto const parameter_variable_declaration_list_def = -(variable_declaration % ",");

	BOOST_SPIRIT_DEFINE(
		variable_definition,
		variable_declaration,
		multiple_variable_declaration,
		variable_declaration_list,
		parameter_variable_declaration_list
	)
}

#endif /* !ELANG_DECLARATION_PARSER_H */
