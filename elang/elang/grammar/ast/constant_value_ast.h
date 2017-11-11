#pragma once

#ifndef ELANG_CONSTANT_VALUE_AST_H
#define ELANG_CONSTANT_VALUE_AST_H

#include "../../common/constant_value.h"
#include "../../vm/machine.h"

#include "asm_ast.h"

ELANG_AST_BEGIN

ELANG_AST_DECLARE_SINGLE(constant_value, elang::common::constant_value)

struct constant_value_traverser{
	elang::easm::instruction::operand_base::ptr_type operator()(const constant_value &ast) const{
		return elang::vm::machine::compiler.get_constant_operand(ast.value);
	}
};

ELANG_AST_END

ELANG_AST_ADAPT_SINGLE(constant_value)

#endif /* !ELANG_CONSTANT_VALUE_AST_H */
