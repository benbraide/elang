#pragma once

#ifndef ELANG_ASM_AST_H
#define ELANG_ASM_AST_H

#include "../../asm/instruction_id.h"
#include "../../asm/instruction_section.h"

#include "../../asm/instruction_operand/constant_value_instruction_operand.h"
#include "../../asm/instruction_operand/string_value_instruction_operand.h"
#include "../../asm/instruction_operand/register_instruction_operand.h"
#include "../../asm/instruction_operand/label_instruction_operand.h"
#include "../../asm/instruction_operand/memory_instruction_operand.h"
#include "../../asm/instruction_operand/expression_instruction_operand.h"
#include "../../asm/instruction_operand/grouped_expression_instruction_operand.h"

#include "../../asm/instruction/nop_instruction.h"
#include "../../asm/instruction/arithmetic_instruction.h"
#include "../../asm/instruction/cmp_instruction.h"
#include "../../asm/instruction/call_instruction.h"
#include "../../asm/instruction/ret_instruction.h"
#include "../../asm/instruction/jmp_instruction.h"
#include "../../asm/instruction/loop_instruction.h"
#include "../../asm/instruction/mov_instruction.h"
#include "../../asm/instruction/stack_instruction.h"
#include "../../asm/instruction/test_instruction.h"
#include "../../asm/instruction/times_instruction.h"
#include "../../asm/instruction/decl_instruction.h"

#include "../grammar_utils.h"

#define ELANG_AST_CREATE_INSTRUCTION(name)\
return std::make_shared<elang::easm::instruction::name>(std::move(operands));

ELANG_AST_BEGIN

using instruction_operand_ptr_type = elang::easm::instruction::operand_base::ptr_type;
using instruction_ptr_type = elang::easm::instruction::base::ptr_type;

ELANG_AST_DECLARE_SINGLE_WPOS(asm_integral_value, __int64)
ELANG_AST_DECLARE_SINGLE_WPOS(asm_float_value, long double)

ELANG_AST_DECLARE_SINGLE_WPOS(asm_string, std::vector<char>)

using asm_identifier = elang::grammar::elang_identifier_ast;

ELANG_AST_DECLARE_PAIR(asm_absolute_identifier, asm_identifier, std::vector<asm_identifier>)

ELANG_AST_DECLARE_SINGLE_WPOS(asm_section, elang::easm::section_id)
ELANG_AST_DECLARE_SINGLE(asm_label, asm_identifier)
ELANG_AST_DECLARE_PAIR_WPOS(asm_relative_label, std::vector<char>, asm_label)

using asm_expression = instruction_operand_ptr_type;

ELANG_AST_DECLARE_SINGLE(asm_grouped_expression, asm_expression)

using asm_sized_memory_variant = boost::variant<asm_identifier, asm_absolute_identifier>;

ELANG_AST_DECLARE_SINGLE_WPOS(asm_memory, asm_expression)
ELANG_AST_DECLARE_PAIR(asm_sized_memory, asm_memory, asm_sized_memory_variant)

ELANG_AST_DECLARE_SINGLE_VARIANT(asm_expression_operand, asm_integral_value, asm_float_value, asm_identifier, asm_absolute_identifier, asm_grouped_expression)
ELANG_AST_DECLARE_SINGLE_VARIANT(asm_operand, asm_string, asm_expression, asm_memory, asm_sized_memory)
ELANG_AST_DECLARE_PAIR_WPOS(asm_typed_operand, elang::vm::machine_value_type_id, asm_operand)

using asm_instruction_variant = boost::variant<asm_operand, asm_typed_operand>;

ELANG_AST_DECLARE_PAIR_WPOS(asm_instruction, elang::easm::instruction::id, std::vector<asm_instruction_variant>)
ELANG_AST_DECLARE_PAIR_WPOS(asm_times_instruction, unsigned int, asm_instruction)

struct asm_type_def;

ELANG_AST_DECLARE_SINGLE_WPOS(asm_struct_def_value, x3::forward_ast<asm_type_def>)
ELANG_AST_DECLARE_SINGLE_WPOS(asm_struct_def, std::vector<asm_struct_def_value>)

using asm_type_def_variant = boost::variant<elang::vm::machine_value_type_id, asm_struct_def>;

ELANG_AST_DECLARE_PAIR_WPOS(asm_type_def, asm_identifier, asm_type_def_variant)

ELANG_AST_DECLARE_SINGLE_VARIANT(asm_instruction_set_value, asm_section, asm_label, asm_relative_label, asm_instruction, asm_times_instruction, asm_type_def)
ELANG_AST_DECLARE_SINGLE(asm_instruction_set, std::vector<asm_instruction_set_value>)

struct asm_traverser{
	static void translate(const asm_instruction_set &ast){
		for (auto &entry : ast.value)
			boost::apply_visitor(asm_traverser(), entry.value);
	}

	static instruction_operand_ptr_type operand(const asm_expression_operand &ast){
		return nullptr;
	}

	void operator()(const asm_section &ast) const{
		
	}

	void operator()(const asm_label &ast) const{

	}

	void operator()(const asm_relative_label &ast) const{

	}

	void operator()(const asm_instruction &ast) const{

	}

	void operator()(const asm_times_instruction &ast) const{

	}

	void operator()(const asm_type_def &ast) const{

	}
};

ELANG_AST_END

ELANG_AST_ADAPT_SINGLE(asm_integral_value)
ELANG_AST_ADAPT_SINGLE(asm_float_value)

ELANG_AST_ADAPT_SINGLE(asm_string)
ELANG_AST_ADAPT_PAIR(asm_absolute_identifier)

ELANG_AST_ADAPT_SINGLE(asm_section)
ELANG_AST_ADAPT_SINGLE(asm_label)
ELANG_AST_ADAPT_PAIR(asm_relative_label)

ELANG_AST_ADAPT_SINGLE(asm_grouped_expression)

ELANG_AST_ADAPT_SINGLE(asm_memory)
ELANG_AST_ADAPT_PAIR(asm_sized_memory)

ELANG_AST_ADAPT_SINGLE(asm_expression_operand)
ELANG_AST_ADAPT_SINGLE(asm_operand)
ELANG_AST_ADAPT_PAIR(asm_typed_operand)

ELANG_AST_ADAPT_PAIR(asm_instruction)
ELANG_AST_ADAPT_PAIR(asm_times_instruction)

ELANG_AST_ADAPT_SINGLE(asm_struct_def_value)
ELANG_AST_ADAPT_SINGLE(asm_struct_def)
ELANG_AST_ADAPT_PAIR(asm_type_def)

ELANG_AST_ADAPT_SINGLE(asm_instruction_set_value)
ELANG_AST_ADAPT_SINGLE(asm_instruction_set)

#endif /* !ELANG_ASM_AST_H */
