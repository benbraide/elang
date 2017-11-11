#pragma once

#ifndef ELANG_IDENTIFIER_AST_H
#define ELANG_IDENTIFIER_AST_H

#include "../../vm/machine.h"
#include "../../common/raii.h"

#include "asm_ast.h"
#include "operator_symbol_ast.h"

ELANG_AST_BEGIN

using identifier = elang::grammar::elang_identifier_ast;

struct non_operator_identifier_compatible;

ELANG_AST_DECLARE_SINGLE_VARIANT(operator_identifier, operator_symbol, x3::forward_ast<non_operator_identifier_compatible>)

ELANG_AST_DECLARE_SINGLE_VARIANT(global_qualified_identifier, identifier, operator_identifier)

using qualified_identifier_variant = boost::variant<identifier, operator_identifier, global_qualified_identifier>;
using qualified_identifier_rvariant = boost::variant<identifier, operator_identifier>;

ELANG_AST_DECLARE_PAIR(qualified_identifier, qualified_identifier_variant, std::vector<qualified_identifier_rvariant>)

ELANG_AST_DECLARE_SINGLE_VARIANT(identifier_compatible, identifier, operator_identifier, global_qualified_identifier, qualified_identifier)
ELANG_AST_DECLARE_SINGLE_VARIANT(non_operator_identifier_compatible, identifier, global_qualified_identifier, qualified_identifier)

struct identifier_resolver_helper{
	static elang::vm::symbol_entry *resolve(const operator_identifier &ast);
};

struct identifier_resolver{
	elang::vm::symbol_entry *operator ()(const identifier &ast) const{
		std::string id;
		utils::identifier_to_string(ast, id);
		return elang::vm::machine::compiler.find(id);
	}

	elang::vm::symbol_entry *operator ()(const operator_identifier &ast) const{
		return identifier_resolver_helper::resolve(ast);
	}

	elang::vm::symbol_entry *operator ()(const global_qualified_identifier &ast) const{
		typedef elang::vm::compiler::current_context_info_type current_context_info_type;
		auto &info = elang::vm::machine::compiler.info();
		elang::common::raii_state<current_context_info_type> raii(info.current_context, current_context_info_type{ info.global_context.get(), false });//Use global context
		return boost::apply_visitor(identifier_resolver(), ast.value);
	}

	elang::vm::symbol_entry *operator ()(const qualified_identifier &ast) const{
		elang::vm::storage_symbol_entry *context;
		auto value = boost::apply_visitor(identifier_resolver(), ast.first);

		auto &info = elang::vm::machine::compiler.info();
		auto old_context = info.current_context;

		try{
			info.current_context.bubble_search = false;
			for (auto &rast : ast.second){
				if (value == nullptr)
					throw elang::vm::compiler_error::undefined;

				if ((context = dynamic_cast<elang::vm::storage_symbol_entry *>(value)) == nullptr)
					throw elang::vm::compiler_error::storage_expected;

				info.current_context.value = context;//Use context
				value = boost::apply_visitor(identifier_resolver(), rast);
			}

			info.current_context = old_context;//Restore context
		}
		catch (...){
			info.current_context = old_context;//Restore context
			throw;//Forward exception
		}

		return value;
	}

	elang::vm::symbol_entry *operator ()(const qualified_identifier_variant &ast) const{
		return boost::apply_visitor(identifier_resolver(), ast);
	}

	elang::vm::symbol_entry *operator ()(const qualified_identifier_rvariant &ast) const{
		return boost::apply_visitor(identifier_resolver(), ast);
	}

	elang::vm::symbol_entry *operator ()(const identifier_compatible &ast) const{
		return boost::apply_visitor(identifier_resolver(), ast.value);
	}

	elang::vm::symbol_entry *operator ()(const non_operator_identifier_compatible &ast) const{
		return boost::apply_visitor(identifier_resolver(), ast.value);
	}
};

struct operator_identifier_resolver{
	template <typename id_type>
	static std::string mangle(const id_type &ast){
		auto entry = identifier_resolver()(ast);
		if (entry == nullptr)
			throw elang::vm::compiler_error::undefined;

		if (entry->id() != elang::vm::symbol_entry_id::type)
			throw elang::vm::compiler_error::type_expected;

		return ("@" + entry->mangle());
	}

	static std::string mangle(const operator_symbol &ast){
		return ("@" + elang::common::utils::mangle_operator_symbol(ast.first, std::string(ast.second.data(), ast.second.size())));
	}

	template <typename id_type>
	elang::vm::symbol_entry *operator ()(const id_type &ast) const{
		return elang::vm::machine::compiler.find(mangle(ast));
	}
};

struct identifier_traverser{
	template <typename id_type>
	elang::easm::instruction::operand_base::ptr_type operator()(const id_type &ast) const{
		auto entry = identifier_resolver()(ast);
		if (entry == nullptr)//Entry not found
			throw elang::vm::compiler_error::undefined;

		if (entry->id() != elang::vm::symbol_entry_id::variable)
			throw elang::vm::compiler_error::variable_expected;

		return dynamic_cast<elang::vm::variable_symbol_entry *>(entry)->reference();
	}
};

ELANG_AST_END

ELANG_AST_ADAPT_SINGLE(operator_identifier)

ELANG_AST_ADAPT_SINGLE(global_qualified_identifier)
ELANG_AST_ADAPT_PAIR(qualified_identifier)

ELANG_AST_ADAPT_SINGLE(identifier_compatible)
ELANG_AST_ADAPT_SINGLE(non_operator_identifier_compatible)

#endif /* !ELANG_IDENTIFIER_AST_H */
