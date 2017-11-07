#pragma once

#ifndef ELANG_CONSTANT_VALUE_AST_H
#define ELANG_CONSTANT_VALUE_AST_H

#include "../../common/constant_value.h"

#include "asm_ast.h"

ELANG_AST_BEGIN

ELANG_AST_DECLARE_SINGLE(constant_value, elang::common::constant_value)

struct constant_value_traverser{
	instruction_operand_ptr_type operator()(const constant_value &ast) const{
		switch (ast.value){
		case elang::common::constant_value::false_:
			return std::make_shared<elang::easm::instruction::label_operand>("#false", std::vector<std::string>{});
		case elang::common::constant_value::true_:
			return std::make_shared<elang::easm::instruction::label_operand>("#true", std::vector<std::string>{});
		case elang::common::constant_value::indeterminate:
			return std::make_shared<elang::easm::instruction::label_operand>("#ind", std::vector<std::string>{});
		case elang::common::constant_value::nullptr_:
			return std::make_shared<elang::easm::instruction::label_operand>("#null", std::vector<std::string>{});
		case elang::common::constant_value::nan_:
			return std::make_shared<elang::easm::instruction::label_operand>("#nan", std::vector<std::string>{});
		case elang::common::constant_value::undefined_:
			return std::make_shared<elang::easm::instruction::label_operand>("#und", std::vector<std::string>{});
		case elang::common::constant_value::infinite_:
			return std::make_shared<elang::easm::instruction::label_operand>("#inf", std::vector<std::string>{});
		default:
			break;
		}

		throw elang::easm::instruction_error::bad_instruction;
	}
};

ELANG_AST_END

ELANG_AST_ADAPT_SINGLE(constant_value)

#endif /* !ELANG_CONSTANT_VALUE_AST_H */
