#pragma once

#ifndef ELANG_CONSTANT_VALUE_AST_H
#define ELANG_CONSTANT_VALUE_AST_H

#include "ast_common.h"

ELANG_AST_BEGIN

ELANG_AST_DECLARE_SINGLE(constant_value, elang::common::constant_value)

struct constant_value_traverser{
	ELANG_AST_COMMON_TRAVERSER_BEGIN(constant_value_traverser)

	void operator()(const constant_value &ast) const{
		switch (ast.value){
		case elang::common::constant_value::false_:
		case elang::common::constant_value::true_:
		case elang::common::constant_value::indeterminate:
			ELANG_AST_COMMON_TRAVERSER_OUT->type = elang::vm::machine::compiler.find_primitive_type(elang::common::primitive_type_id::bool_);
			break;
		case elang::common::constant_value::nullptr_:
			ELANG_AST_COMMON_TRAVERSER_OUT->type = elang::vm::machine::compiler.find_primitive_type(elang::common::primitive_type_id::nullptr_);
			break;
		default:
			throw elang::vm::compiler_error::unreachable;
			break;
		}

		ELANG_AST_COMMON_TRAVERSER_OUT->value = ast.value;
		ELANG_AST_COMMON_TRAVERSER_OUT->is_constant = true;
		ELANG_AST_COMMON_TRAVERSER_OUT->is_static = true;
	}
};

ELANG_AST_END

ELANG_AST_ADAPT_SINGLE(constant_value)

#endif /* !ELANG_CONSTANT_VALUE_AST_H */
