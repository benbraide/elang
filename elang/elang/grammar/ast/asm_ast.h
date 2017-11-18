#pragma once

#ifndef ELANG_ASM_AST_H
#define ELANG_ASM_AST_H

#include "../../vm/machine.h"

#include "../../asm/instruction_id.h"
#include "../../asm/instruction_section.h"

#include "../../asm/instruction_operand/uninitialized_value_instruction_operand.h"
#include "../../asm/instruction_operand/constant_value_instruction_operand.h"
#include "../../asm/instruction_operand/string_value_instruction_operand.h"
#include "../../asm/instruction_operand/register_instruction_operand.h"
#include "../../asm/instruction_operand/label_instruction_operand.h"
#include "../../asm/instruction_operand/offset_instruction_operand.h"
#include "../../asm/instruction_operand/memory_instruction_operand.h"
#include "../../asm/instruction_operand/expression_instruction_operand.h"
#include "../../asm/instruction_operand/grouped_expression_instruction_operand.h"

#include "../../asm/instruction/nop_instruction.h"
#include "../../asm/instruction/arithmetic_instruction.h"
#include "../../asm/instruction/cmp_instruction.h"
#include "../../asm/instruction/call_instruction.h"
#include "../../asm/instruction/ret_instruction.h"
#include "../../asm/instruction/jmp_instruction.h"
#include "../../asm/instruction/set_instruction.h"
#include "../../asm/instruction/loop_instruction.h"
#include "../../asm/instruction/mov_instruction.h"
#include "../../asm/instruction/lea_instruction.h"
#include "../../asm/instruction/stack_instruction.h"
#include "../../asm/instruction/test_instruction.h"
#include "../../asm/instruction/syscall_instruction.h"
#include "../../asm/instruction/times_instruction.h"
#include "../../asm/instruction/decl_instruction.h"

#include "../../asm/instruction/extended/extended_mov_instruction.h"
#include "../../asm/instruction/extended/extended_arithmetic_instruction.h"
#include "../../asm/instruction/extended/extended_cmp_instruction.h"
#include "../../asm/instruction/extended/extended_stack_instruction.h"
#include "../../asm/instruction/extended/extended_syscall_instruction.h"

#include "../grammar_utils.h"

#define ELANG_AST_CREATE_INSTRUCTION(name)\
return std::make_shared<elang::easm::instruction::name>(std::move(operands));

ELANG_AST_BEGIN

using instruction_operand_ptr_type = elang::easm::instruction::operand_base::ptr_type;
using instruction_ptr_type = elang::easm::instruction::base::ptr_type;

ELANG_AST_DECLARE_SINGLE_WPOS(asm_uninitialized_value, char)

ELANG_AST_DECLARE_SINGLE_WPOS(asm_integral_value, __int64)
ELANG_AST_DECLARE_SINGLE_WPOS(asm_float_value, long double)

ELANG_AST_DECLARE_SINGLE_WPOS(asm_string, std::vector<char>)

using asm_identifier = elang::grammar::elang_identifier_ast;

ELANG_AST_DECLARE_SINGLE(asm_global_qualified_identifier, asm_identifier)

using asm_qualified_identifier_variant = boost::variant<asm_identifier, asm_global_qualified_identifier>;

ELANG_AST_DECLARE_PAIR(asm_qualified_identifier, asm_qualified_identifier_variant, std::vector<asm_identifier>)
ELANG_AST_DECLARE_SINGLE_VARIANT(asm_identifier_compatible, asm_identifier, asm_global_qualified_identifier, asm_qualified_identifier)

ELANG_AST_DECLARE_PAIR(asm_absolute_identifier, asm_identifier_compatible, std::vector<asm_identifier_compatible>)

ELANG_AST_DECLARE_SINGLE_WPOS(asm_section, elang::easm::section_id)
ELANG_AST_DECLARE_PAIR_WPOS(asm_label, std::vector<char>, asm_identifier_compatible)

using asm_expression = instruction_operand_ptr_type;

ELANG_AST_DECLARE_SINGLE(asm_grouped_expression, asm_expression)

using asm_sized_memory_variant = boost::variant<asm_identifier, asm_absolute_identifier>;

ELANG_AST_DECLARE_SINGLE_WPOS(asm_memory, asm_expression)
ELANG_AST_DECLARE_PAIR(asm_sized_memory, asm_memory, asm_sized_memory_variant)

ELANG_AST_DECLARE_SINGLE_VARIANT(asm_expression_operand, asm_integral_value, asm_float_value, asm_string, asm_identifier_compatible, asm_absolute_identifier, asm_grouped_expression)
ELANG_AST_DECLARE_SINGLE_VARIANT(asm_operand, asm_integral_value, asm_float_value, asm_string, asm_identifier_compatible, asm_absolute_identifier, asm_memory, asm_sized_memory)
ELANG_AST_DECLARE_PAIR_WPOS(asm_typed_operand, elang::vm::machine_value_type_id, asm_operand)

using asm_instruction_variant = boost::variant<asm_uninitialized_value, asm_expression, asm_operand, asm_typed_operand>;

ELANG_AST_DECLARE_PAIR_WPOS(asm_instruction, elang::easm::instruction::id, std::vector<asm_instruction_variant>)
ELANG_AST_DECLARE_PAIR_WPOS(asm_extended_instruction, elang::easm::instruction::id, std::vector<asm_instruction_variant>)

using asm_times_instruction_variant = boost::variant<asm_instruction, asm_extended_instruction>;

ELANG_AST_DECLARE_PAIR_WPOS(asm_times_instruction, unsigned int, asm_times_instruction_variant)

struct asm_type_def;

ELANG_AST_DECLARE_SINGLE_WPOS(asm_struct_def_value, x3::forward_ast<asm_type_def>)
ELANG_AST_DECLARE_SINGLE_WPOS(asm_struct_def, std::vector<asm_struct_def_value>)

using asm_type_def_variant = boost::variant<elang::vm::machine_value_type_id, asm_struct_def>;

ELANG_AST_DECLARE_PAIR_WPOS(asm_type_def, asm_identifier, asm_type_def_variant)

ELANG_AST_DECLARE_SINGLE_VARIANT(asm_instruction_set_value, asm_section, asm_label, asm_instruction, asm_extended_instruction, asm_times_instruction, asm_type_def)
ELANG_AST_DECLARE_SINGLE(asm_instruction_set, std::vector<asm_instruction_set_value>)

struct asm_identifier_to_string{
	explicit asm_identifier_to_string(std::string &out)
		: out_(&out){}

	void operator ()(const asm_identifier &ast) const{
		utils::identifier_to_string(ast, *out_);
	}

	void operator ()(const asm_global_qualified_identifier &ast) const{
		operator ()(ast.value);
		*out_ = ("::" + *out_);
	}

	void operator ()(const asm_qualified_identifier &ast) const{
		boost::apply_visitor(asm_identifier_to_string(*out_), ast.first);
		for (auto &entry : ast.second){
			*out_ += "::";
			operator()(entry);
		}
	}

	void operator ()(const asm_identifier_compatible &ast) const{
		boost::apply_visitor(asm_identifier_to_string(*out_), ast.value);
	}

	std::string *out_;
};

struct asm_traverser{
	static void translate(const asm_instruction_set &ast){
		for (auto &entry : ast.value)
			boost::apply_visitor(asm_traverser(), entry.value);
	}

	static instruction_ptr_type instruction(elang::easm::instruction::id id, const std::vector<instruction_operand_ptr_type> &operands){
		switch (id){
		case elang::easm::instruction::id::nop:
			return std::make_shared<elang::easm::instruction::nop>(std::move(operands));
		case elang::easm::instruction::id::ret:
			return std::make_shared<elang::easm::instruction::ret>(std::move(operands));
		case elang::easm::instruction::id::push:
			return std::make_shared<elang::easm::instruction::push>(std::move(operands));
		case elang::easm::instruction::id::pop:
			return std::make_shared<elang::easm::instruction::pop>(std::move(operands));
		case elang::easm::instruction::id::int_:
			return std::make_shared<elang::easm::instruction::syscall>(std::move(operands));
		case elang::easm::instruction::id::call:
			return std::make_shared<elang::easm::instruction::call>(std::move(operands));
		case elang::easm::instruction::id::jmp:
			return std::make_shared<elang::easm::instruction::jmp>(std::move(operands));
		case elang::easm::instruction::id::jz:
			return std::make_shared<elang::easm::instruction::jz>(std::move(operands));
		case elang::easm::instruction::id::jnz:
			return std::make_shared<elang::easm::instruction::jnz>(std::move(operands));
		case elang::easm::instruction::id::je:
			return std::make_shared<elang::easm::instruction::je>(std::move(operands));
		case elang::easm::instruction::id::jne:
			return std::make_shared<elang::easm::instruction::jne>(std::move(operands));
		case elang::easm::instruction::id::jle:
			return std::make_shared<elang::easm::instruction::jle>(std::move(operands));
		case elang::easm::instruction::id::jl:
			return std::make_shared<elang::easm::instruction::jl>(std::move(operands));
		case elang::easm::instruction::id::jnle:
			return std::make_shared<elang::easm::instruction::jnle>(std::move(operands));
		case elang::easm::instruction::id::jnl:
			return std::make_shared<elang::easm::instruction::jnl>(std::move(operands));
		case elang::easm::instruction::id::jge:
			return std::make_shared<elang::easm::instruction::jge>(std::move(operands));
		case elang::easm::instruction::id::jg:
			return std::make_shared<elang::easm::instruction::jg>(std::move(operands));
		case elang::easm::instruction::id::jnge:
			return std::make_shared<elang::easm::instruction::jnge>(std::move(operands));
		case elang::easm::instruction::id::jng:
			return std::make_shared<elang::easm::instruction::jng>(std::move(operands));
		case elang::easm::instruction::id::loop:
			return std::make_shared<elang::easm::instruction::loop>(std::move(operands));
		case elang::easm::instruction::id::inc:
			return std::make_shared<elang::easm::instruction::inc>(std::move(operands));
		case elang::easm::instruction::id::dec:
			return std::make_shared<elang::easm::instruction::dec>(std::move(operands));
		case elang::easm::instruction::id::not:
			return std::make_shared<elang::easm::instruction::not>(std::move(operands));
		case elang::easm::instruction::id::lea:
			return std::make_shared<elang::easm::instruction::lea>(std::move(operands));
		case elang::easm::instruction::id::mov:
			return std::make_shared<elang::easm::instruction::mov>(std::move(operands));
		case elang::easm::instruction::id::add:
			return std::make_shared<elang::easm::instruction::add>(std::move(operands));
		case elang::easm::instruction::id::sub:
			return std::make_shared<elang::easm::instruction::sub>(std::move(operands));
		case elang::easm::instruction::id::mult:
			return std::make_shared<elang::easm::instruction::mult>(std::move(operands));
		case elang::easm::instruction::id::div:
			return std::make_shared<elang::easm::instruction::div>(std::move(operands));
		case elang::easm::instruction::id::mod:
			return std::make_shared<elang::easm::instruction::mod>(std::move(operands));
		case elang::easm::instruction::id::and_:
			return std::make_shared<elang::easm::instruction::and_>(std::move(operands));
		case elang::easm::instruction::id::xor_:
			return std::make_shared<elang::easm::instruction::xor_>(std::move(operands));
		case elang::easm::instruction::id::or_:
			return std::make_shared<elang::easm::instruction::or_>(std::move(operands));
		case elang::easm::instruction::id::sal:
			return std::make_shared<elang::easm::instruction::sal>(std::move(operands));
		case elang::easm::instruction::id::sar:
			return std::make_shared<elang::easm::instruction::sar>(std::move(operands));
		case elang::easm::instruction::id::test:
			return std::make_shared<elang::easm::instruction::test>(std::move(operands));
		case elang::easm::instruction::id::cmp:
			return std::make_shared<elang::easm::instruction::cmp>(std::move(operands));
		case elang::easm::instruction::id::db:
			return std::make_shared<elang::easm::instruction::db>(std::move(operands));
		case elang::easm::instruction::id::dw:
			return std::make_shared<elang::easm::instruction::dw>(std::move(operands));
		case elang::easm::instruction::id::dd:
			return std::make_shared<elang::easm::instruction::dd>(std::move(operands));
		case elang::easm::instruction::id::dq:
			return std::make_shared<elang::easm::instruction::dq>(std::move(operands));
		case elang::easm::instruction::id::df:
			return std::make_shared<elang::easm::instruction::df>(std::move(operands));
		default:
			break;
		}

		throw elang::easm::instruction_error::bad_instruction;
	}

	static instruction_ptr_type instruction(const asm_instruction &ast){
		std::vector<instruction_operand_ptr_type> operands;
		asm_traverser::operands(ast.second, operands);
		return instruction(ast.first, operands);
	}

	static instruction_ptr_type instruction(const asm_extended_instruction &ast){
		std::vector<instruction_operand_ptr_type> operands;
		asm_traverser::operands(ast.second, operands);

		switch (ast.first){
		case elang::easm::instruction::id::mov:
			return std::make_shared<elang::easm::instruction::extended_mov>(std::move(operands));
		case elang::easm::instruction::id::add:
			return std::make_shared<elang::easm::instruction::ex_add>(std::move(operands));
		case elang::easm::instruction::id::sub:
			return std::make_shared<elang::easm::instruction::ex_sub>(std::move(operands));
		case elang::easm::instruction::id::mult:
			return std::make_shared<elang::easm::instruction::ex_mult>(std::move(operands));
		case elang::easm::instruction::id::div:
			return std::make_shared<elang::easm::instruction::ex_div>(std::move(operands));
		case elang::easm::instruction::id::mod:
			return std::make_shared<elang::easm::instruction::ex_mod>(std::move(operands));
		case elang::easm::instruction::id::and_:
			return std::make_shared<elang::easm::instruction::ex_and_>(std::move(operands));
		case elang::easm::instruction::id::xor_:
			return std::make_shared<elang::easm::instruction::ex_xor_>(std::move(operands));
		case elang::easm::instruction::id::or_:
			return std::make_shared<elang::easm::instruction::ex_or_>(std::move(operands));
		case elang::easm::instruction::id::sal:
			return std::make_shared<elang::easm::instruction::ex_sal>(std::move(operands));
		case elang::easm::instruction::id::sar:
			return std::make_shared<elang::easm::instruction::ex_sar>(std::move(operands));
		case elang::easm::instruction::id::cmp:
			return std::make_shared<elang::easm::instruction::extended_cmp>(std::move(operands));
		case elang::easm::instruction::id::push:
			return std::make_shared<elang::easm::instruction::extended_push>(std::move(operands));
		case elang::easm::instruction::id::pop:
			return std::make_shared<elang::easm::instruction::extended_pop>(std::move(operands));
		case elang::easm::instruction::id::int_:
			return std::make_shared<elang::easm::instruction::extended_syscall>(std::move(operands));
		default:
			break;
		}

		return instruction(ast.first, operands);
	}

	static instruction_operand_ptr_type operand(const asm_instruction_variant &var){
		return boost::apply_visitor(asm_traverser(), var);
	}

	static instruction_operand_ptr_type operand(const asm_expression_operand &ast){
		return boost::apply_visitor(asm_traverser(), ast.value);
	}

	static void operands(const std::vector<asm_instruction_variant> &asts, std::vector<instruction_operand_ptr_type> &out){
		if (asts.empty())
			return;

		out.reserve(asts.size());
		for (auto &ast : asts)
			out.push_back(operand(ast));
	}

	void operator()(const asm_section &ast) const{
		elang::vm::machine::asm_translation.add(ast.value);
	}

	void operator()(const asm_label &ast) const{
		std::string label;
		asm_identifier_to_string idts(label);
		idts(ast.second);
		elang::vm::machine::asm_translation.add(static_cast<unsigned int>(ast.first.size()), label);
	}

	void operator()(const asm_instruction &ast) const{
		elang::vm::machine::asm_translation.add(instruction(ast));
	}

	void operator()(const asm_extended_instruction &ast) const{
		elang::vm::machine::asm_translation.add(instruction(ast));
	}

	void operator()(const asm_times_instruction &ast) const{
		instruction_ptr_type value;
		if (ast.second.which() == 0)
			value = instruction(boost::get<asm_instruction>(ast.second));
		else//Extended
			value = instruction(boost::get<asm_extended_instruction>(ast.second));

		elang::vm::machine::asm_translation.add(std::make_shared<elang::easm::instruction::times>(ast.first, value));
	}

	void operator()(const asm_type_def &ast) const{

	}

	instruction_operand_ptr_type operator()(const asm_operand &ast) const{
		return boost::apply_visitor(asm_traverser(), ast.value);
	}

	instruction_operand_ptr_type operator()(const asm_typed_operand &ast) const{
		auto value = operator()(ast.second);
		value->apply_value_type(ast.first);
		return value;
	}

	instruction_operand_ptr_type operator()(const asm_string &ast) const{
		std::string value(ast.value.data(), ast.value.size());
		return std::make_shared<elang::easm::instruction::string_value_operand>(std::move(value));
	}

	instruction_operand_ptr_type operator()(const asm_expression &ast) const{
		return ast;
	}

	instruction_operand_ptr_type operator()(const asm_memory &ast) const{
		return std::make_shared<elang::easm::instruction::memory_operand>(elang::vm::machine_value_type_id::unknown, ast.value);
	}

	instruction_operand_ptr_type operator()(const asm_sized_memory &ast) const{
		return nullptr;//#TODO: Implement
	}

	instruction_operand_ptr_type operator()(const asm_uninitialized_value &ast) const{
		return std::make_shared<elang::easm::instruction::uninitialized_value_operand>();
	}

	instruction_operand_ptr_type operator()(const asm_integral_value &ast) const{
		return std::make_shared<elang::easm::instruction::constant_value_operand<__int64>>(elang::vm::machine_value_type_id::unknown, ast.value);
	}

	instruction_operand_ptr_type operator()(const asm_float_value &ast) const{
		return std::make_shared<elang::easm::instruction::constant_value_operand<long double>>(elang::vm::machine_value_type_id::unknown, ast.value);
	}

	instruction_operand_ptr_type operator()(const asm_identifier &ast) const{
		std::string id;
		utils::identifier_to_string(ast, id);
		if (id == "$")//Offset operand
			return std::make_shared<elang::easm::instruction::offset_operand>();

		if (id.size() < 5u){//Check for register
			auto reg = elang::vm::machine::register_manager.find(id, true);
			if (reg != nullptr)//Register operand
				return std::make_shared<elang::easm::instruction::register_operand>(*reg);
		}
		
		return std::make_shared<elang::easm::instruction::label_operand>(id);
	}

	instruction_operand_ptr_type operator()(const asm_global_qualified_identifier &ast) const{
		if (!elang::vm::machine::compiler.is_compiling()){
			std::string id;
			asm_identifier_to_string idts(id);
			idts(ast);
			return std::make_shared<elang::easm::instruction::label_operand>(id);
		}

		return nullptr;
	}

	instruction_operand_ptr_type operator()(const asm_qualified_identifier &ast) const{
		if (!elang::vm::machine::compiler.is_compiling()){
			std::string id;
			asm_identifier_to_string idts(id);
			idts(ast);
			return std::make_shared<elang::easm::instruction::label_operand>(id);
		}

		return nullptr;
	}

	instruction_operand_ptr_type operator()(const asm_identifier_compatible &ast) const{
		return boost::apply_visitor(asm_traverser(), ast.value);
	}

	instruction_operand_ptr_type operator()(const asm_absolute_identifier &ast) const{
		std::string first;
		boost::apply_visitor(asm_identifier_to_string(first), ast.first.value);

		auto index = 0u;
		std::vector<std::string> rest(ast.second.size());

		for (auto &entry : ast.second)//Resolve rest
			boost::apply_visitor(asm_identifier_to_string(rest[index++]), entry.value);

		return std::make_shared<elang::easm::instruction::label_operand>(std::move(first), std::move(rest));
	}

	instruction_operand_ptr_type operator()(const asm_grouped_expression &ast) const{
		return std::make_shared<elang::easm::instruction::grouped_expression_operand>(ast.value);
	}
};

ELANG_AST_END

ELANG_AST_ADAPT_SINGLE(asm_uninitialized_value)

ELANG_AST_ADAPT_SINGLE(asm_integral_value)
ELANG_AST_ADAPT_SINGLE(asm_float_value)
ELANG_AST_ADAPT_SINGLE(asm_string)

ELANG_AST_ADAPT_SINGLE(asm_global_qualified_identifier)
ELANG_AST_ADAPT_PAIR(asm_qualified_identifier)

ELANG_AST_ADAPT_SINGLE(asm_identifier_compatible)
ELANG_AST_ADAPT_PAIR(asm_absolute_identifier)

ELANG_AST_ADAPT_SINGLE(asm_section)
ELANG_AST_ADAPT_PAIR(asm_label)

ELANG_AST_ADAPT_SINGLE(asm_grouped_expression)

ELANG_AST_ADAPT_SINGLE(asm_memory)
ELANG_AST_ADAPT_PAIR(asm_sized_memory)

ELANG_AST_ADAPT_SINGLE(asm_expression_operand)
ELANG_AST_ADAPT_SINGLE(asm_operand)
ELANG_AST_ADAPT_PAIR(asm_typed_operand)

ELANG_AST_ADAPT_PAIR(asm_instruction)
ELANG_AST_ADAPT_PAIR(asm_extended_instruction)
ELANG_AST_ADAPT_PAIR(asm_times_instruction)

ELANG_AST_ADAPT_SINGLE(asm_struct_def_value)
ELANG_AST_ADAPT_SINGLE(asm_struct_def)
ELANG_AST_ADAPT_PAIR(asm_type_def)

ELANG_AST_ADAPT_SINGLE(asm_instruction_set_value)
ELANG_AST_ADAPT_SINGLE(asm_instruction_set)

#endif /* !ELANG_ASM_AST_H */
