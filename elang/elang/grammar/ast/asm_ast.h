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

struct asm_absolute_identifier{
	asm_identifier first;
	std::vector<asm_identifier> rest;
};

ELANG_AST_DECLARE_SINGLE_WPOS(asm_section, elang::easm::section_id)
ELANG_AST_DECLARE_SINGLE(asm_label, asm_identifier)

struct asm_relative_label{
	std::vector<char> magnitude;
	asm_label label;
};

ELANG_AST_DECLARE_SINGLE(asm_relative_label, asm_label)

using asm_expression = instruction_operand_ptr_type;

ELANG_AST_DECLARE_SINGLE(asm_grouped_expression, asm_expression)

struct asm_operand;

ELANG_AST_DECLARE_SINGLE_WPOS(asm_memory, x3::forward_ast<asm_operand>)

ELANG_AST_DECLARE_SINGLE_VARIANT(asm_operand, asm_integral_value, asm_float_value, asm_identifier, asm_absolute_identifier, asm_expression, asm_grouped_expression, asm_memory)

struct asm_typed_operand{
	elang::vm::machine_value_type_id type;
	asm_operand operand;
};

struct asm_instruction{
	typedef boost::variant<asm_operand, asm_typed_operand> operand_type;
	elang::easm::instruction::id id;
	std::vector<operand_type> operands;
};

struct asm_times_instruction{
	unsigned int count;
	asm_instruction instruction;
};

ELANG_AST_DECLARE_SINGLE_VARIANT(asm_instruction_set, asm_instruction, asm_times_instruction)

struct asm_traverser{
	static instruction_operand_ptr_type operand(const asm_operand &ast){
		return nullptr;
	}
};

ELANG_AST_END

ELANG_AST_ADAPT_SINGLE(asm_integral_value)
ELANG_AST_ADAPT_SINGLE(asm_float_value)

ELANG_AST_ADAPT_SINGLE(asm_string)

BOOST_FUSION_ADAPT_STRUCT(
	elang::grammar::ast::asm_absolute_identifier,
	(elang::grammar::ast::asm_identifier, first)
	(std::vector<elang::grammar::ast::asm_identifier>, rest)
)

ELANG_AST_ADAPT_SINGLE(asm_section)
ELANG_AST_ADAPT_SINGLE(asm_label)

BOOST_FUSION_ADAPT_STRUCT(
	elang::grammar::ast::asm_relative_label,
	(std::vector<char>, magnitude)
	(elang::grammar::ast::asm_label, label)
)

ELANG_AST_ADAPT_SINGLE(asm_grouped_expression)
ELANG_AST_ADAPT_SINGLE(asm_memory)

ELANG_AST_ADAPT_SINGLE(asm_operand)

BOOST_FUSION_ADAPT_STRUCT(
	elang::grammar::ast::asm_typed_operand,
	(elang::vm::machine_value_type_id, type)
	(elang::grammar::ast::asm_operand, operand)
)

BOOST_FUSION_ADAPT_STRUCT(
	elang::grammar::ast::asm_instruction,
	(elang::easm::instruction::id, id)
	(std::vector<elang::grammar::ast::asm_instruction::operand_type>, operands)
)

BOOST_FUSION_ADAPT_STRUCT(
	elang::grammar::ast::asm_times_instruction,
	(unsigned int, count)
	(elang::grammar::ast::asm_instruction, instruction)
)

ELANG_AST_ADAPT_SINGLE(asm_instruction_set)

#endif /* !ELANG_ASM_AST_H */
