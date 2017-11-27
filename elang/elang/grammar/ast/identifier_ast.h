#pragma once

#ifndef ELANG_IDENTIFIER_AST_H
#define ELANG_IDENTIFIER_AST_H

#include "../../common/raii.h"

#include "ast_common.h"
#include "operator_symbol_ast.h"

ELANG_AST_BEGIN

using identifier = elang::grammar::elang_identifier_ast;

struct non_operator_identifier_compatible;

ELANG_AST_DECLARE_SINGLE_VARIANT(operator_identifier, elang::common::operator_id, operator_symbol, x3::forward_ast<non_operator_identifier_compatible>)

ELANG_AST_DECLARE_SINGLE_VARIANT(global_qualified_identifier, identifier, operator_identifier)

using qualified_identifier_variant = boost::variant<identifier, operator_identifier, global_qualified_identifier>;

ELANG_AST_DECLARE_SINGLE(qualified_identifier, std::vector<qualified_identifier_variant>)

ELANG_AST_DECLARE_SINGLE_VARIANT(identifier_compatible, identifier, operator_identifier, global_qualified_identifier, qualified_identifier)
ELANG_AST_DECLARE_SINGLE_VARIANT(non_operator_identifier_compatible, identifier, global_qualified_identifier, qualified_identifier)

struct storage_resolver{
	explicit storage_resolver(bool storage_only, elang::vm::storage_symbol_entry *search_context = nullptr)
		: storage_only_(storage_only), search_context_(search_context){}

	template <typename ast_type>
	elang::vm::symbol_entry *operator ()(const ast_type &ast) const{
		return nullptr;
	}

	elang::vm::symbol_entry *operator ()(const identifier &ast) const{
		std::string id;
		utils::identifier_to_string(ast, id);
		if (storage_only_)//Find storage entries only
			return ((search_context_ == nullptr) ? elang::vm::machine::compiler.find_storage(id) : search_context_->find_storage(id));
		return ((search_context_ == nullptr) ? elang::vm::machine::compiler.find_storage_or_any(id) : search_context_->find_storage_or_any(id));
	}

	elang::vm::symbol_entry *operator ()(const global_qualified_identifier &ast) const{
		return boost::apply_visitor(storage_resolver(storage_only_, elang::vm::machine::compiler.info().global_context.get()), ast.value);
	}

	elang::vm::symbol_entry *operator ()(const qualified_identifier &ast) const{
		auto search_context = search_context_;
		auto iter = ast.value.begin(), last = std::prev(ast.value.end());

		for (; iter != last; ++iter){//Resolver context
			auto context = boost::apply_visitor(storage_resolver(false, search_context), *iter);
			if (context == nullptr)//End search
				return nullptr;

			if (!context->is_storage())//Error
				throw elang::vm::compiler_error::storage_expected;

			search_context = dynamic_cast<elang::vm::storage_symbol_entry *>(context);//Use context
		}

		return boost::apply_visitor(storage_resolver(storage_only_, search_context), *iter);
	}

	elang::vm::symbol_entry *operator ()(const qualified_identifier_variant &ast) const{
		return boost::apply_visitor(*this, ast);
	}

	elang::vm::symbol_entry *operator ()(const identifier_compatible &ast) const{
		return boost::apply_visitor(*this, ast.value);
	}

	elang::vm::symbol_entry *operator ()(const non_operator_identifier_compatible &ast) const{
		return boost::apply_visitor(*this, ast.value);
	}

	bool storage_only_;
	elang::vm::storage_symbol_entry *search_context_;
};

struct identifier_resolver_helper{
	static elang::vm::symbol_entry *resolve(const operator_identifier &ast, elang::vm::storage_symbol_entry *search_context);
};

struct identifier_resolver{
	explicit identifier_resolver(elang::vm::storage_symbol_entry *search_context = nullptr)
		: search_context_(search_context){}

	elang::vm::symbol_entry *operator ()(const identifier &ast) const{
		std::string id;
		utils::identifier_to_string(ast, id);
		return ((search_context_ == nullptr) ? elang::vm::machine::compiler.find(id) : search_context_->find(id));
	}

	elang::vm::symbol_entry *operator ()(const operator_identifier &ast) const{
		return identifier_resolver_helper::resolve(ast, search_context_);
	}

	elang::vm::symbol_entry *operator ()(const global_qualified_identifier &ast) const{
		return boost::apply_visitor(identifier_resolver(elang::vm::machine::compiler.info().global_context.get()), ast.value);
	}

	elang::vm::symbol_entry *operator ()(const qualified_identifier &ast) const{
		auto search_context = search_context_;
		auto iter = ast.value.begin(), last = std::prev(ast.value.end());

		for (; iter != last; ++iter){//Resolver context
			auto context = boost::apply_visitor(storage_resolver(false, search_context), *iter);
			if (context == nullptr)//End search
				return nullptr;

			if (!context->is_storage())//Error
				throw elang::vm::compiler_error::storage_expected;

			search_context = dynamic_cast<elang::vm::storage_symbol_entry *>(context);//Use context
		}

		return boost::apply_visitor(identifier_resolver(search_context), *iter);
	}

	elang::vm::symbol_entry *operator ()(const qualified_identifier_variant &ast) const{
		return boost::apply_visitor(*this, ast);
	}

	elang::vm::symbol_entry *operator ()(const identifier_compatible &ast) const{
		return boost::apply_visitor(*this, ast.value);
	}

	elang::vm::symbol_entry *operator ()(const non_operator_identifier_compatible &ast) const{
		return boost::apply_visitor(*this, ast.value);
	}

	elang::vm::storage_symbol_entry *search_context_;
};

struct operator_identifier_resolver{
	explicit operator_identifier_resolver(elang::vm::storage_symbol_entry *search_context = nullptr)
		: search_context_(search_context){}

	template <typename id_type>
	static std::string mangle(const id_type &ast){
		storage_resolver resolver(false, search_context_);
		auto entry = resolver(ast);
		if (entry == nullptr)
			throw elang::vm::compiler_error::undefined;

		if (entry->id() != elang::vm::symbol_entry_id::type)
			throw elang::vm::compiler_error::type_expected;

		return (elang::vm::machine::compiler.mangle(elang::vm::mangle_target::operator_) + entry->mangle());
	}

	static std::string mangle(elang::common::operator_id id){
		return (elang::vm::machine::compiler.mangle(elang::vm::mangle_target::operator_) + elang::common::utils::mangle_operator_symbol(id));
	}

	static std::string mangle(const operator_symbol &ast){
		return (elang::vm::machine::compiler.mangle(elang::vm::mangle_target::operator_) +
			elang::common::utils::mangle_operator_symbol(ast.first, std::string(ast.second.data(), ast.second.size())));
	}

	template <typename id_type>
	elang::vm::symbol_entry *operator ()(const id_type &ast) const{
		return ((search_context_ == nullptr) ? elang::vm::machine::compiler.find(mangle(ast)) : search_context_->find(mangle(ast)));
	}

	elang::vm::storage_symbol_entry *search_context_;
};

struct identifier_traverser{
	ELANG_AST_COMMON_TRAVERSER_BEGIN(identifier_traverser)

	template <typename id_type>
	void operator()(const id_type &ast) const{
		auto entry = identifier_resolver()(ast);
		if (entry == nullptr)//Entry not found
			throw elang::vm::compiler_error::undefined;

		if (entry->id() != elang::vm::symbol_entry_id::variable)
			throw elang::vm::compiler_error::variable_expected;

		auto this_ = ELANG_AST_COMMON_TRAVERSER_OUT;
		auto variable_entry = reinterpret_cast<elang::vm::variable_symbol_entry *>(entry);

		this_->type = variable_entry->type();
		if (ELANG_IS(variable_entry->attributes(), elang::vm::symbol_entry_attribute::static_const) &&
			(this_->type->is_numeric() || this_->type->is_null_pointer() || this_->type->is_pointer())){//Resolve value
			this_->is_constant = this_->is_static = true;
			if (this_->type->is_integral() || this_->type->is_null_pointer() || this_->type->is_pointer()){
				auto id = (this_->type->is_integral() ? elang::common::primitive_type_id::int64_ : elang::common::primitive_type_id::pointer);
				this_->value = integer_value_info{ elang::vm::machine::compiler.get_static_const_value<__int64>(*variable_entry), id };
			}
			else//Float
				this_->value = elang::vm::machine::compiler.get_static_const_value<long double>(*variable_entry);
		}
		else{//Resolve at runtime
			this_->is_static = false;
			this_->is_constant = this_->type->is_const();
			if (ELANG_IS(variable_entry->attributes(), elang::vm::symbol_entry_attribute::static_))
				this_->value = memory_operand_value_info{ variable_entry->mangle(), variable_entry->stack_offset(), variable_entry->size() };
			else//No label
				this_->value = memory_operand_value_info{ "", variable_entry->stack_offset(), variable_entry->size() };
		}
	}
};

ELANG_AST_END

ELANG_AST_ADAPT_SINGLE(operator_identifier)

ELANG_AST_ADAPT_SINGLE(global_qualified_identifier)
ELANG_AST_ADAPT_SINGLE(qualified_identifier)

ELANG_AST_ADAPT_SINGLE(identifier_compatible)
ELANG_AST_ADAPT_SINGLE(non_operator_identifier_compatible)

#endif /* !ELANG_IDENTIFIER_AST_H */
