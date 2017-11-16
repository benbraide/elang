#pragma once

#ifndef ELANG_AST_COMMON_H
#define ELANG_AST_COMMON_H

#include "../../common/numeric_literal_suffix.h"
#include "../../common/string_quote_type.h"
#include "../../common/constant_value.h"

#include "../../vm/machine.h"

#include "asm_ast.h"

#define ELANG_AST_COMMON_TRAVERSER_BEGIN(n)			\
													\
operand_value_info *out_;							\
													\
explicit n(operand_value_info &out) : out_(&out){}

#define ELANG_AST_COMMON_TRAVERSER_OUT out_

#define ELANG_AST_COMMON_TRAVERSER_OUT_DREF (*(ELANG_AST_COMMON_TRAVERSER_OUT))

ELANG_AST_BEGIN

struct string_operand_value_info{
	std::string value;
	bool is_wide;
};

using operand_value_type = boost::variant<
	elang::common::constant_value,
	__int64,
	long double,
	string_operand_value_info,
	elang::vm::symbol_entry *,
	elang::vm::machine_register *,
	elang::easm::instruction::operand_base::ptr_type
>;

struct operand_value_info{
	operand_value_type value;
	elang::vm::type_info::ptr_type type;
	bool is_constant;
	bool is_static;
};

ELANG_AST_END

#endif /* !ELANG_AST_COMMON_H */
