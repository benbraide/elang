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
