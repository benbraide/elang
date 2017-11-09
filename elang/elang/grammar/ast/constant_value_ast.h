#pragma once

#ifndef ELANG_CONSTANT_VALUE_AST_H
#define ELANG_CONSTANT_VALUE_AST_H

#include "../../common/constant_value.h"
#include "../../vm/machine.h"

#include "asm_ast.h"

ELANG_AST_BEGIN

ELANG_AST_DECLARE_SINGLE(constant_value, elang::common::constant_value)

struct constant_value_traverser{
	void operator()(const constant_value &ast) const{
		using instruction_operand_ptr_type = elang::easm::instruction::operand_base::ptr_type;

		std::string label;
		elang::vm::machine_value_type_id size;

		switch (ast.value){
		case elang::common::constant_value::false_:
			label = "__false__";
			size = elang::vm::machine_value_type_id::byte;
			break;
		case elang::common::constant_value::true_:
			label = "__true__";
			size = elang::vm::machine_value_type_id::byte;
			break;
		case elang::common::constant_value::indeterminate:
			label = "__ind__";
			size = elang::vm::machine_value_type_id::byte;
			break;
		case elang::common::constant_value::nullptr_:
			label = "__null__";
			size = elang::vm::machine_value_type_id::qword;
			break;
		case elang::common::constant_value::nan_:
			label = "__nan__";
			size = elang::vm::machine_value_type_id::qword;
			break;
		case elang::common::constant_value::infinite_:
			label = "__inf__";
			size = elang::vm::machine_value_type_id::qword;
			break;
		default:
			throw elang::easm::instruction_error::bad_instruction;
			break;
		}

		auto reg = elang::vm::machine::compiler.store().get(size);
		if (reg == nullptr)//Error
			throw elang::vm::machine_error::no_register;

		auto reg_op = std::make_shared<elang::easm::instruction::register_operand>(*reg);
		auto label_op = std::make_shared<elang::easm::instruction::label_operand>(label, std::vector<std::string>());

		auto mem_op = std::make_shared<elang::easm::instruction::memory_operand>(elang::vm::machine_value_type_id::unknown, label_op);
		auto instruction = std::make_shared<elang::easm::instruction::mov>(std::vector<instruction_operand_ptr_type>({ reg_op, mem_op }));

		elang::vm::machine::compiler.section(elang::easm::section_id::text).add(instruction);
	}
};

ELANG_AST_END

ELANG_AST_ADAPT_SINGLE(constant_value)

#endif /* !ELANG_CONSTANT_VALUE_AST_H */
