#pragma once

#ifndef ELANG_TYPE_AST_H
#define ELANG_TYPE_AST_H

#include "../../common/primitive_type_id.h"

#include "identifier_ast.h"

ELANG_AST_BEGIN

ELANG_AST_DECLARE_SINGLE(primitive_type, elang::common::primitive_type_id)
ELANG_AST_DECLARE_SINGLE(user_type, elang::vm::symbol_entry *)

ELANG_AST_DECLARE_SINGLE_VARIANT(const_storage_type, primitive_type, user_type)

using pointer_type_variant = boost::variant<primitive_type, user_type, const_storage_type>;

ELANG_AST_DECLARE_PAIR(pointer_type, pointer_type_variant, std::vector<char>)

ELANG_AST_DECLARE_SINGLE_VARIANT(array_type, primitive_type, user_type, const_storage_type, pointer_type)

ELANG_AST_DECLARE_SINGLE_VARIANT(ref_type, primitive_type, user_type, const_storage_type, pointer_type)
ELANG_AST_DECLARE_SINGLE_VARIANT(vref_type, primitive_type, user_type, const_storage_type, pointer_type)

ELANG_AST_DECLARE_SINGLE_VARIANT(const_target_type, primitive_type, user_type, const_storage_type, pointer_type, array_type, ref_type, vref_type)

ELANG_AST_DECLARE_SINGLE_VARIANT(type, primitive_type, user_type, const_storage_type, pointer_type, array_type, ref_type, vref_type, const_target_type)
ELANG_AST_DECLARE_SINGLE(variadic_type, type)

struct type_traverser{
	elang::vm::type_info::ptr_type operator()(const primitive_type &ast) const{
		return elang::vm::machine::compiler.find_primitive_type(ast.value);
	}

	elang::vm::type_info::ptr_type operator()(const user_type &ast) const{
		return std::make_shared<elang::vm::user_type_info>(ast.value->reflect(), elang::vm::type_info::attribute_type::nil);
	}

	elang::vm::type_info::ptr_type operator()(const const_storage_type &ast) const{
		auto value = boost::apply_visitor(type_traverser(), ast.value);
		return value->clone(value->attributes() | elang::vm::type_info::attribute_type::const_);
	}

	elang::vm::type_info::ptr_type operator()(const pointer_type &ast) const{
		auto value = boost::apply_visitor(type_traverser(), ast.first);
		for (auto count = ast.second.size(); count > 0u; --count)
			value = std::make_shared<elang::vm::pointer_type_info>(value, elang::vm::type_info::attribute_type::nil);
		return value;
	}

	elang::vm::type_info::ptr_type operator()(const array_type &ast) const{
		auto value = boost::apply_visitor(type_traverser(), ast.value);
		return std::make_shared<elang::vm::array_type_info>(value, 0u, elang::vm::type_info::attribute_type::nil);
	}

	elang::vm::type_info::ptr_type operator()(const ref_type &ast) const{
		auto value = boost::apply_visitor(type_traverser(), ast.value);
		return value->clone(value->attributes() | elang::vm::type_info::attribute_type::ref_);
	}

	elang::vm::type_info::ptr_type operator()(const vref_type &ast) const{
		auto value = boost::apply_visitor(type_traverser(), ast.value);
		return value->clone(value->attributes() | elang::vm::type_info::attribute_type::vref);
	}

	elang::vm::type_info::ptr_type operator()(const const_target_type &ast) const{
		auto value = boost::apply_visitor(type_traverser(), ast.value);
		if (!value->is_pointer())//Constant storage
			return value->clone(value->attributes() | elang::vm::type_info::attribute_type::const_);

		auto underlying_type = value->underlying_type();
		auto modified_underlying_type = underlying_type->clone(underlying_type->attributes() | elang::vm::type_info::attribute_type::const_);

		return std::make_shared<elang::vm::pointer_type_info>(modified_underlying_type, value->attributes());
	}

	elang::vm::type_info::ptr_type operator()(const type &ast) const{
		return boost::apply_visitor(type_traverser(), ast.value);
	}

	elang::vm::type_info::ptr_type operator()(const variadic_type &ast) const{
		return std::make_shared<elang::vm::variadic_type_info>(operator()(ast.value), elang::vm::type_info::attribute_type::nil);
	}
};

ELANG_AST_END

ELANG_AST_ADAPT_SINGLE(primitive_type)
ELANG_AST_ADAPT_SINGLE(user_type)

ELANG_AST_ADAPT_SINGLE(const_storage_type)

ELANG_AST_ADAPT_PAIR(pointer_type)
ELANG_AST_ADAPT_SINGLE(array_type)

ELANG_AST_ADAPT_SINGLE(ref_type)
ELANG_AST_ADAPT_SINGLE(vref_type)

ELANG_AST_ADAPT_SINGLE(const_target_type)

ELANG_AST_ADAPT_SINGLE(type)
ELANG_AST_ADAPT_SINGLE(variadic_type)

#endif /* !ELANG_TYPE_AST_H */
