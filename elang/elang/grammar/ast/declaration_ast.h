#pragma once

#ifndef ELANG_DECLARATION_AST_H
#define ELANG_DECLARATION_AST_H

#include "type_ast.h"
#include "expression_ast.h"

ELANG_AST_BEGIN

ELANG_AST_DECLARE_SINGLE(storage_class, elang::vm::symbol_entry_attribute)

struct variable_definition{
	std::vector<ast::storage_class> storage_class;
	ast::type type;
	non_operator_identifier_compatible id;
	boost::optional<expression> init;
};

struct variable_declaration{
	typedef boost::variant<variadic_type, ast::type> variant_type;
	std::vector<ast::storage_class> storage_class;
	variant_type type;
	boost::optional<identifier> id;
	boost::optional<non_comma_expression> init;
};

struct multiple_variable_declaration{
	std::vector<char> pointer_depth;
	boost::optional<bool> is_ref;
	identifier id;
	boost::optional<non_comma_expression> init;
};

ELANG_AST_DECLARE_PAIR(variable_declaration_list, variable_declaration, std::vector<multiple_variable_declaration>)
ELANG_AST_DECLARE_SINGLE(parameter_variable_declaration_list, std::vector<variable_declaration>)

struct declaration_traverser{
	void operator ()(const variable_definition &ast) const{
		if (!ast.storage_class.empty())
			throw elang::vm::compiler_error::bad_storage_class;

		

		type_traverser type_traverser_inst;
		auto type_value = type_traverser_inst(ast.type);
	}

	void operator ()(const variable_declaration &ast) const{
		typedef elang::vm::type_info::attribute_type attribute_type;
		if (ast.storage_class.size() > 2u)
			throw elang::vm::compiler_error::bad_storage_class;

		if (!ast.id.is_initialized())
			throw elang::vm::compiler_error::id_required;

		auto attributes = elang::vm::symbol_entry_attribute::nil;
		for (auto &attr : ast.storage_class)//Combine attributes
			ELANG_SET(attributes, attr.value);

		if (ELANG_IS(attributes, elang::vm::symbol_entry_attribute::tls) && !ELANG_IS(attributes, elang::vm::symbol_entry_attribute::static_))
			throw elang::vm::compiler_error::bad_storage_class;

		auto type_value = boost::apply_visitor(type_traverser(), ast.type);
		if (type_value == nullptr || type_value->is_variadic())
			throw elang::vm::compiler_error::bad_type;

		std::string id_value;
		utils::identifier_to_string(ast.id.get(), id_value);

		elang::common::raii_state<bool> disable_bubble(elang::vm::machine::compiler.info().current_context.bubble_search, false);
		if (elang::vm::machine::compiler.find(id_value) != nullptr)
			throw elang::vm::compiler_error::redefinition;

		operand_value_info init_value{};
		auto is_class = (dynamic_cast<elang::vm::class_type_symbol_entry *>(elang::vm::machine::compiler.info().current_context.value) != nullptr);

		if (ast.init.is_initialized()){
			expression_traverser expr(init_value);
			expr(ast.init);//Evaluate initialization
			if (type_value->primitive_id() != elang::common::primitive_type_id::auto_ && !type_value->is_same_unmodified(*init_value.type));

			if (ELANG_IS(type_value->attributes(), attribute_type::ref_)){

			}
			
		}
		else if (type_value->primitive_id() == elang::common::primitive_type_id::auto_)//Requires initialization
			throw elang::vm::compiler_error::initialization_required;
		else if (!is_class && ELANG_IS_ANY(type_value->attributes(), attribute_type::const_ | attribute_type::ref_ | attribute_type::vref))
			throw elang::vm::compiler_error::initialization_required;//Requires initialization
		else if (ELANG_IS(attributes, elang::vm::symbol_entry_attribute::static_))//Explicit definition required
			ELANG_SET(attributes, elang::vm::symbol_entry_attribute::undefined_);

		elang::vm::machine::compiler.info().current_context.value->add_variable(id_value, type_value, attributes);
	}

	void operator ()(const multiple_variable_declaration &ast) const{

	}

	void operator ()(const variable_declaration_list &ast) const{
		operator ()(ast.first);
	}

	void operator ()(const parameter_variable_declaration_list &ast) const{

	}
};

ELANG_AST_END

BOOST_FUSION_ADAPT_STRUCT(
	elang::grammar::ast::variable_definition,
	(std::vector<elang::grammar::ast::storage_class>, storage_class)
	(elang::grammar::ast::type, type)
	(elang::grammar::ast::non_operator_identifier_compatible, id)
	(boost::optional<elang::grammar::ast::expression>, init)
)

BOOST_FUSION_ADAPT_STRUCT(
	elang::grammar::ast::variable_declaration,
	(std::vector<elang::grammar::ast::storage_class>, storage_class)
	(elang::grammar::ast::variable_declaration::variant_type, type)
	(boost::optional<elang::grammar::ast::identifier>, id)
	(boost::optional<elang::grammar::ast::non_comma_expression>, init)
)

BOOST_FUSION_ADAPT_STRUCT(
	elang::grammar::ast::multiple_variable_declaration,
	(std::vector<char>, pointer_depth)
	(boost::optional<bool>, is_ref)
	(elang::grammar::ast::identifier, id)
	(boost::optional<elang::grammar::ast::non_comma_expression>, init)
)

ELANG_AST_ADAPT_PAIR(variable_declaration_list)
ELANG_AST_ADAPT_SINGLE(parameter_variable_declaration_list)

#endif /* !ELANG_DECLARATION_AST_H */
