#pragma once

#ifndef ELANG_EXPRESSION_AST_H
#define ELANG_EXPRESSION_AST_H

#include "type_ast.h"
#include "identifier_ast.h"
#include "literal_value_ast.h"
#include "constant_value_ast.h"
#include "operator_symbol_ast.h"

#define ELANG_AST_EXPR_DECL_FACTOR															\
ELANG_AST_DECLARE_PAIR(factor, elang::common::operator_id, term)							\
ELANG_AST_DECLARE_PAIR(factor_expression, term, std::vector<factor>)

#define ELANG_AST_EXPR_DECL(name, higher)													\
																							\
using full_ ## name ## _variant = boost::variant<term, higher ## _expression>;				\
																							\
ELANG_AST_DECLARE_PAIR(full_ ## name, elang::common::operator_id, full_ ## name ## _variant)\
																							\
ELANG_AST_DECLARE_SINGLE_VARIANT(name, higher, full_ ## name)								\
																							\
ELANG_AST_DECLARE_PAIR(name ## _expression, term, std::vector<name>)

#define ELANG_AST_EXPR_ADAPT_FACTOR															\
ELANG_AST_ADAPT_PAIR(factor)																\
ELANG_AST_ADAPT_PAIR(factor_expression)

#define ELANG_AST_EXPR_ADAPT(name)															\
ELANG_AST_ADAPT_PAIR(full_ ## name)															\
ELANG_AST_ADAPT_SINGLE(name)																\
ELANG_AST_ADAPT_PAIR(name ## _expression)

ELANG_AST_BEGIN

using instruction_operand_ptr_type = elang::easm::instruction::operand_base::ptr_type;

struct expression;

ELANG_AST_DECLARE_SINGLE(grouped_expression, x3::forward_ast<expression>)

ELANG_AST_DECLARE_PAIR(static_cast_expression, type, grouped_expression)
ELANG_AST_DECLARE_PAIR(reinterpret_cast_expression, type, grouped_expression)
ELANG_AST_DECLARE_PAIR(dynamic_cast_expression, type, grouped_expression)

ELANG_AST_DECLARE_SINGLE_VARIANT(cast_expression, static_cast_expression, reinterpret_cast_expression, dynamic_cast_expression)

ELANG_AST_DECLARE_SINGLE_VARIANT(non_operator_term, constant_value, literal_value, identifier_compatible, grouped_expression, cast_expression)

ELANG_AST_DECLARE_PAIR(member_access, elang::common::operator_id, identifier_compatible)
ELANG_AST_DECLARE_SINGLE(call, boost::optional<x3::forward_ast<expression>>)
ELANG_AST_DECLARE_SINGLE(index, x3::forward_ast<expression>)

using right_unary_term_variant = boost::variant<elang::common::operator_id, member_access, call, index>;

ELANG_AST_DECLARE_PAIR(right_unary_term, non_operator_term, std::vector<right_unary_term_variant>)

using left_unary_term_variant = boost::variant<non_operator_term, right_unary_term>;

ELANG_AST_DECLARE_PAIR(left_unary_term, elang::common::operator_id, left_unary_term_variant)
ELANG_AST_DECLARE_SINGLE_VARIANT(term, non_operator_term, right_unary_term, left_unary_term)

ELANG_AST_EXPR_DECL_FACTOR

ELANG_AST_EXPR_DECL(addition, factor)
ELANG_AST_EXPR_DECL(shift, addition)

ELANG_AST_EXPR_DECL(relation, shift)
ELANG_AST_EXPR_DECL(equality, relation)

ELANG_AST_EXPR_DECL(bitwise_and, equality)
ELANG_AST_EXPR_DECL(bitwise_xor, bitwise_and)
ELANG_AST_EXPR_DECL(bitwise_or, bitwise_xor)

ELANG_AST_EXPR_DECL(logical_and, equality)
ELANG_AST_EXPR_DECL(logical_or, logical_and)

struct right_associative_expression;
struct ternary;

using right_associative_variant = boost::variant<term, logical_and_expression, x3::forward_ast<right_associative_expression>, x3::forward_ast<ternary>>;

ELANG_AST_DECLARE_PAIR(full_right_associative, elang::common::operator_id, right_associative_variant)
ELANG_AST_DECLARE_SINGLE_VARIANT(right_associative, logical_and_expression, full_right_associative)
ELANG_AST_DECLARE_PAIR(right_associative_expression, term, std::vector<right_associative>)

struct comma_expression;

using ternary_variant = boost::variant<term, x3::forward_ast<comma_expression>, right_associative_expression>;

ELANG_AST_DECLARE_PAIR(full_ternary, ternary_variant, ternary_variant)
ELANG_AST_DECLARE_SINGLE_VARIANT(ternary, right_associative, full_ternary)
ELANG_AST_DECLARE_PAIR(ternary_expression, term, std::vector<ternary>)

using comma_variant = boost::variant<term, ternary_expression>;

ELANG_AST_DECLARE_PAIR(full_comma, elang::common::operator_id, comma_variant)
ELANG_AST_DECLARE_SINGLE_VARIANT(comma, ternary, full_comma)
ELANG_AST_DECLARE_PAIR(comma_expression, term, std::vector<comma>)

ELANG_AST_DECLARE_SINGLE_VARIANT(expression, term, comma_expression)
ELANG_AST_DECLARE_SINGLE_VARIANT(non_comma_expression, term, ternary_expression)

struct static_evaluator{
	static void set_boolean_value(operand_value_info &info, bool value){
		info.value = (value ? elang::common::constant_value::true_ : elang::common::constant_value::false_);
		info.type = elang::vm::machine::compiler.find_primitive_type(elang::common::primitive_type_id::bool_);
	}

	static void integral(elang::common::operator_id op, operand_value_info &left, operand_value_info &right, bool is_boolean){
		if (is_boolean){
			integral_boolean(op, left, right);
			return;
		}

		__int64 computed;
		switch (op){
		case elang::common::operator_id::plus:
			computed = (boost::apply_visitor(get_int_value(), left.value) + boost::apply_visitor(get_int_value(), right.value));
			break;
		case elang::common::operator_id::minus:
			computed = (boost::apply_visitor(get_int_value(), left.value) - boost::apply_visitor(get_int_value(), right.value));
			break;
		case elang::common::operator_id::times:
			computed = (boost::apply_visitor(get_int_value(), left.value) * boost::apply_visitor(get_int_value(), right.value));
			break;
		case elang::common::operator_id::divide:
		{
			auto right_value = boost::apply_visitor(get_int_value(), right.value);
			if (right_value != 0)
				computed = (boost::apply_visitor(get_int_value(), left.value) / right_value);
			else//Error
				throw elang::vm::machine_error::division_by_zero;
			break;
		}
		case elang::common::operator_id::modulus:
		{
			auto right_value = boost::apply_visitor(get_int_value(), right.value);
			if (right_value != 0)
				computed = (boost::apply_visitor(get_int_value(), left.value) % right_value);
			else//Error
				throw elang::vm::machine_error::division_by_zero;
			break;
		}
		case elang::common::operator_id::left_shift:
			computed = static_cast<__int64>(boost::apply_visitor(get_uint_value(), left.value) << boost::apply_visitor(get_uint_value(), right.value));
			break;
		case elang::common::operator_id::right_shift:
			computed = static_cast<__int64>(boost::apply_visitor(get_uint_value(), left.value) >> boost::apply_visitor(get_uint_value(), right.value));
			break;
		case elang::common::operator_id::bitwise_and:
			computed = static_cast<__int64>(boost::apply_visitor(get_uint_value(), left.value) & boost::apply_visitor(get_uint_value(), right.value));
			break;
		case elang::common::operator_id::bitwise_xor:
			computed = static_cast<__int64>(boost::apply_visitor(get_uint_value(), left.value) ^ boost::apply_visitor(get_uint_value(), right.value));
			break;
		case elang::common::operator_id::bitwise_or:
			computed = static_cast<__int64>(boost::apply_visitor(get_uint_value(), left.value) | boost::apply_visitor(get_uint_value(), right.value));
			break;
		default:
			throw elang::vm::compiler_error::invalid_operation;
			break;
		}

		left.type = ((left.type->primitive_id() < right.type->primitive_id()) ? right.type : left.type);
		left.value = integer_value_info{ computed, left.type->primitive_id() };
	}

	static void integral_boolean(elang::common::operator_id op, operand_value_info &left, operand_value_info &right){
		switch (op){
		case elang::common::operator_id::less:
			set_boolean_value(left, boost::apply_visitor(get_int_value(), left.value) < boost::apply_visitor(get_int_value(), right.value));
			break;
		case elang::common::operator_id::less_or_equal:
			set_boolean_value(left, boost::apply_visitor(get_int_value(), left.value) <= boost::apply_visitor(get_int_value(), right.value));
			break;
		case elang::common::operator_id::equality:
			set_boolean_value(left, boost::apply_visitor(get_int_value(), left.value) == boost::apply_visitor(get_int_value(), right.value));
			break;
		case elang::common::operator_id::inverse_equality:
			set_boolean_value(left, boost::apply_visitor(get_int_value(), left.value) != boost::apply_visitor(get_int_value(), right.value));
			break;
		case elang::common::operator_id::greater_or_equal:
			set_boolean_value(left, boost::apply_visitor(get_int_value(), left.value) >= boost::apply_visitor(get_int_value(), right.value));
			break;
		case elang::common::operator_id::greater:
			set_boolean_value(left, boost::apply_visitor(get_int_value(), left.value) > boost::apply_visitor(get_int_value(), right.value));
			break;
		default:
			throw elang::vm::compiler_error::invalid_operation;
			break;
		}
	}

	static void numeric(elang::common::operator_id op, operand_value_info &left, operand_value_info &right, bool is_boolean){
		if (is_boolean){
			numeric_boolean(op, left, right);
			return;
		}

		switch (op){
		case elang::common::operator_id::plus:
			left.value = (boost::apply_visitor(get_float_value(), left.value) + boost::apply_visitor(get_float_value(), right.value));
			break;
		case elang::common::operator_id::minus:
			left.value = (boost::apply_visitor(get_float_value(), left.value) - boost::apply_visitor(get_float_value(), right.value));
			break;
		case elang::common::operator_id::times:
			left.value = (boost::apply_visitor(get_float_value(), left.value) * boost::apply_visitor(get_float_value(), right.value));
			break;
		case elang::common::operator_id::divide:
		{
			auto right_value = boost::apply_visitor(get_float_value(), right.value);
			if (right_value != 0.0l)
				left.value = (boost::apply_visitor(get_float_value(), left.value) / right_value);
			else//Error
				throw elang::vm::machine_error::division_by_zero;
			break;
		}
		default:
			throw elang::vm::compiler_error::invalid_operation;
			break;
		}

		left.type = elang::vm::machine::compiler.find_primitive_type(elang::common::primitive_type_id::float_);
	}

	static void numeric_boolean(elang::common::operator_id op, operand_value_info &left, operand_value_info &right){
		switch (op){
		case elang::common::operator_id::less:
			set_boolean_value(left, boost::apply_visitor(get_float_value(), left.value) < boost::apply_visitor(get_float_value(), right.value));
			break;
		case elang::common::operator_id::less_or_equal:
			set_boolean_value(left, boost::apply_visitor(get_float_value(), left.value) <= boost::apply_visitor(get_float_value(), right.value));
			break;
		case elang::common::operator_id::equality:
			set_boolean_value(left, boost::apply_visitor(get_float_value(), left.value) == boost::apply_visitor(get_float_value(), right.value));
			break;
		case elang::common::operator_id::inverse_equality:
			set_boolean_value(left, boost::apply_visitor(get_float_value(), left.value) != boost::apply_visitor(get_float_value(), right.value));
			break;
		case elang::common::operator_id::greater_or_equal:
			set_boolean_value(left, boost::apply_visitor(get_float_value(), left.value) >= boost::apply_visitor(get_float_value(), right.value));
			break;
		case elang::common::operator_id::greater:
			set_boolean_value(left, boost::apply_visitor(get_float_value(), left.value) > boost::apply_visitor(get_float_value(), right.value));
			break;
		default:
			throw elang::vm::compiler_error::invalid_operation;
			break;
		}
	}

	static void pointer(elang::common::operator_id op, operand_value_info &left, operand_value_info &right, bool is_boolean){
		if (is_boolean){
			pointer_boolean(op, left, right);
			return;
		}

		if (!right.type->is_integral())
			throw elang::vm::compiler_error::invalid_operation;

		__int64 computed;
		switch (op){
		case elang::common::operator_id::plus:
			computed = (boost::apply_visitor(get_int_value(), left.value) + (boost::apply_visitor(get_int_value(), right.value) * 8));
			break;
		case elang::common::operator_id::minus:
			computed = (boost::apply_visitor(get_int_value(), left.value) - (boost::apply_visitor(get_int_value(), right.value) * 8));
			break;
		default:
			throw elang::vm::compiler_error::invalid_operation;
			break;
		}

		left.value = integer_value_info{ computed, elang::common::primitive_type_id::pointer };
	}

	static void pointer_boolean(elang::common::operator_id op, operand_value_info &left, operand_value_info &right){
		if (!left.type->is_null_pointer() && !right.type->is_null_pointer()){//Check types
			if (!right.type->is_pointer())
				throw elang::vm::compiler_error::invalid_operation;

			auto left_underlying_type = left.type->underlying_type(), right_underlying_type = right.type->underlying_type();
			if (!left_underlying_type->is_same(*right_underlying_type) &&
				!left_underlying_type->is_void() &&
				!right_underlying_type->is_void() &&
				!left_underlying_type->is_same_object(*right_underlying_type) &&
				!right_underlying_type->is_same_object(*left_underlying_type)){
				throw elang::vm::compiler_error::invalid_operation;
			}
		}

		integral_boolean(op, left, right);
	}
};

struct runtime_evaluator{
	static void integral(elang::common::operator_id op, operand_value_info &left, operand_value_info &right, bool is_boolean){
		if (is_boolean){
			numeric_boolean(op, left, right);
			return;
		}

		auto left_reg = load_register::load(left);
		auto right_reg = load_register::load(right);

		if (left_reg->type_id() < right_reg->type_id())//Type conversion
			left_reg = load_register::convert_register(*left_reg, right_reg->type_id());
		else if (right_reg->type_id() < left_reg->type_id())//Type conversion
			right_reg = load_register::convert_register(*right_reg, left_reg->type_id());

		switch (op){
		case elang::common::operator_id::plus:
			instruction_creator::add_to_text_section<elang::easm::instruction::add>(*left_reg, *right_reg);
			break;
		case elang::common::operator_id::minus:
			instruction_creator::add_to_text_section<elang::easm::instruction::sub>(*left_reg, *right_reg);
			break;
		case elang::common::operator_id::times:
			instruction_creator::add_to_text_section<elang::easm::instruction::mult>(*left_reg, *right_reg);
			break;
		case elang::common::operator_id::divide:
			instruction_creator::add_to_text_section<elang::easm::instruction::div>(*left_reg, *right_reg);
			break;
		case elang::common::operator_id::modulus:
			instruction_creator::add_to_text_section<elang::easm::instruction::mod>(*left_reg, *right_reg);
			break;
		case elang::common::operator_id::left_shift:
			instruction_creator::add_to_text_section<elang::easm::instruction::sal>(*left_reg, *right_reg);
			break;
		case elang::common::operator_id::right_shift:
			instruction_creator::add_to_text_section<elang::easm::instruction::sar>(*left_reg, *right_reg);
			break;
		case elang::common::operator_id::bitwise_and:
			instruction_creator::add_to_text_section<elang::easm::instruction::and_>(*left_reg, *right_reg);
			break;
		case elang::common::operator_id::bitwise_xor:
			instruction_creator::add_to_text_section<elang::easm::instruction::xor_>(*left_reg, *right_reg);
			break;
		case elang::common::operator_id::bitwise_or:
			instruction_creator::add_to_text_section<elang::easm::instruction::or_>(*left_reg, *right_reg);
			break;
		default:
			throw elang::vm::compiler_error::invalid_operation;
			break;
		}

		elang::vm::machine::compiler.store().put(*right_reg);//Return register
		left.value = left_reg;//Update value
		left.type = ((left.type->primitive_id() < right.type->primitive_id()) ? right.type : left.type);
		left.is_constant = left.is_static = false;//Reset
	}

	static void numeric(elang::common::operator_id op, operand_value_info &left, operand_value_info &right, bool is_boolean){
		if (is_boolean){
			numeric_boolean(op, left, right);
			return;
		}

		auto left_reg = load_register::load(left);
		auto right_reg = load_register::load(right);

		if (left_reg->type_id() < right_reg->type_id())//Type conversion
			left_reg = load_register::convert_register(*left_reg, right_reg->type_id());
		else if (right_reg->type_id() < left_reg->type_id())//Type conversion
			right_reg = load_register::convert_register(*right_reg, left_reg->type_id());

		switch (op){
		case elang::common::operator_id::plus:
			instruction_creator::add_to_text_section<elang::easm::instruction::add>(*left_reg, *right_reg);
			break;
		case elang::common::operator_id::minus:
			instruction_creator::add_to_text_section<elang::easm::instruction::sub>(*left_reg, *right_reg);
			break;
		case elang::common::operator_id::times:
			instruction_creator::add_to_text_section<elang::easm::instruction::mult>(*left_reg, *right_reg);
			break;
		case elang::common::operator_id::divide:
			instruction_creator::add_to_text_section<elang::easm::instruction::div>(*left_reg, *right_reg);
			break;
		default:
			throw elang::vm::compiler_error::invalid_operation;
			break;
		}

		elang::vm::machine::compiler.store().put(*right_reg);//Return register
		left.value = left_reg;//Update value
		left.type = elang::vm::machine::compiler.find_primitive_type(elang::common::primitive_type_id::float_);
		left.is_constant = left.is_static = false;//Reset
	}

	static void numeric_boolean(elang::common::operator_id op, operand_value_info &left, operand_value_info &right){
		auto left_reg = load_register::load(left);
		auto right_reg = load_register::load(right);

		if (!left.type->is_pointer()){
			if (left_reg->type_id() < right_reg->type_id())//Type conversion
				left_reg = load_register::convert_register(*left_reg, right_reg->type_id());
			else if (right_reg->type_id() < left_reg->type_id())//Type conversion
				right_reg = load_register::convert_register(*right_reg, left_reg->type_id());
		}

		instruction_creator::add_to_text_section<elang::easm::instruction::cmp>(*left_reg, *right_reg);
		left_reg = load_register::convert_register(*left_reg, elang::vm::machine_value_type_id::byte);

		switch (op){
		case elang::common::operator_id::less:
			instruction_creator::add_to_text_section<elang::easm::instruction::setl>(*left_reg);
			break;
		case elang::common::operator_id::less_or_equal:
			instruction_creator::add_to_text_section<elang::easm::instruction::setle>(*left_reg);
			break;
		case elang::common::operator_id::equality:
			instruction_creator::add_to_text_section<elang::easm::instruction::sete>(*left_reg);
			break;
		case elang::common::operator_id::inverse_equality:
			instruction_creator::add_to_text_section<elang::easm::instruction::setne>(*left_reg);
			break;
		case elang::common::operator_id::greater_or_equal:
			instruction_creator::add_to_text_section<elang::easm::instruction::setge>(*left_reg);
			break;
		case elang::common::operator_id::greater:
			instruction_creator::add_to_text_section<elang::easm::instruction::setg>(*left_reg);
			break;
		default:
			throw elang::vm::compiler_error::invalid_operation;
			break;
		}

		elang::vm::machine::compiler.store().put(*right_reg);//Return register
		left.value = left_reg;//Update value
		left.type = elang::vm::machine::compiler.find_primitive_type(elang::common::primitive_type_id::bool_);
		left.is_constant = left.is_static = false;//Reset
	}

	static void pointer(elang::common::operator_id op, operand_value_info &left, operand_value_info &right, bool is_boolean){
		auto left_reg = load_register::load(left);
		auto right_reg = load_register::load(right);

		if (right_reg->type_id() != elang::vm::machine_value_type_id::qword)//Type conversion
			right_reg = load_register::convert_register(*right_reg, elang::vm::machine_value_type_id::qword);

		instruction_creator::add_to_text_section<elang::easm::instruction::mult>(*right_reg, static_cast<__int64>(8));
		switch (op){
		case elang::common::operator_id::plus:
			instruction_creator::add_to_text_section<elang::easm::instruction::add>(*left_reg, *right_reg);
			break;
		case elang::common::operator_id::minus:
			instruction_creator::add_to_text_section<elang::easm::instruction::sub>(*left_reg, *right_reg);
			break;
		default:
			throw elang::vm::compiler_error::invalid_operation;
			break;
		}

		elang::vm::machine::compiler.store().put(*right_reg);//Return register
		left.value = left_reg;//Update value
		left.is_constant = left.is_static = false;//Reset
	}

	static void pointer_boolean(elang::common::operator_id op, operand_value_info &left, operand_value_info &right){
		if (!left.type->is_null_pointer() && !right.type->is_null_pointer()){//Check types
			if (!right.type->is_pointer())
				throw elang::vm::compiler_error::invalid_operation;

			auto left_underlying_type = left.type->underlying_type(), right_underlying_type = right.type->underlying_type();
			if (!left_underlying_type->is_same(*right_underlying_type) &&
				!left_underlying_type->is_void() &&
				!right_underlying_type->is_void() &&
				!left_underlying_type->is_same_object(*right_underlying_type) &&
				!right_underlying_type->is_same_object(*left_underlying_type)){
				throw elang::vm::compiler_error::invalid_operation;
			}
		}

		numeric_boolean(op, left, right);
	}
};

struct expression_traverser{
	ELANG_AST_COMMON_TRAVERSER_BEGIN(expression_traverser)

	template <typename ast_type>
	void operator ()(const ast_type &ast) const{
		
	}

	void operator ()(const constant_value &ast) const{
		forward_ast<constant_value_traverser>(ast);
	}

	void operator ()(const literal_value &ast) const{
		forward_ast<literal_value_traverser>(ast);
	}

	void operator ()(const identifier_compatible &ast) const{
		forward_ast<identifier_traverser>(ast);
	}

	void operator ()(const factor &ast) const{
		accumulate(ast);
	}

	void operator ()(const full_addition &ast) const{
		accumulate(ast);
	}

	void operator ()(const addition &ast) const{
		boost::apply_visitor(*this, ast.value);
	}

	void operator ()(const full_shift &ast) const{
		accumulate(ast);
	}

	void operator ()(const shift &ast) const{
		boost::apply_visitor(*this, ast.value);
	}

	void operator ()(const full_relation &ast) const{
		accumulate(ast, true);
	}

	void operator ()(const relation &ast) const{
		boost::apply_visitor(*this, ast.value);
	}

	void operator ()(const full_equality &ast) const{
		accumulate(ast, true);
	}

	void operator ()(const equality &ast) const{
		boost::apply_visitor(*this, ast.value);
	}

	void operator ()(const full_bitwise_and &ast) const{
		accumulate(ast);
	}

	void operator ()(const bitwise_and &ast) const{
		boost::apply_visitor(*this, ast.value);
	}

	void operator ()(const full_bitwise_xor &ast) const{
		accumulate(ast);
	}

	void operator ()(const bitwise_xor &ast) const{
		boost::apply_visitor(*this, ast.value);
	}

	void operator ()(const full_bitwise_or &ast) const{
		accumulate(ast);
	}

	void operator ()(const bitwise_or &ast) const{
		boost::apply_visitor(*this, ast.value);
	}

	void operator ()(const full_logical_and &ast) const{
		accumulate(ast, true);
	}

	void operator ()(const logical_and &ast) const{
		boost::apply_visitor(*this, ast.value);
	}

	void operator ()(const full_logical_or &ast) const{
		accumulate(ast, true);
	}

	void operator ()(const logical_or &ast) const{
		boost::apply_visitor(*this, ast.value);
	}

	void operator ()(const factor_expression &ast) const{
		operator ()(ast.first);//Evaluate left
		for (auto &other_ast : ast.second)//Evaluate cumulative
			accumulate(other_ast);
	}

	void operator ()(const addition_expression &ast) const{
		binary_expression(ast);
	}

	void operator ()(const shift_expression &ast) const{
		binary_expression(ast);
	}

	void operator ()(const relation_expression &ast) const{
		binary_expression(ast);
	}

	void operator ()(const equality_expression &ast) const{
		binary_expression(ast);
	}

	void operator ()(const bitwise_and_expression &ast) const{
		binary_expression(ast);
	}

	void operator ()(const bitwise_xor_expression &ast) const{
		binary_expression(ast);
	}

	void operator ()(const bitwise_or_expression &ast) const{
		binary_expression(ast);
	}

	void operator ()(const logical_and_expression &ast) const{
		binary_expression(ast);
	}

	void operator ()(const logical_or_expression &ast) const{
		binary_expression(ast);
	}

	void operator ()(const static_cast_expression &ast) const{
		operator ()(ast.second);
		if (call_cast_operator(ast.first))
			return;//Operator called

		auto this_ = ELANG_AST_COMMON_TRAVERSER_OUT;
		auto type_value = type_traverser()(ast.first);

		if (type_value->is_ref()){
			if ((this_->type->is_const() && !type_value->is_const()) || !this_->type->is_ref())
				throw elang::vm::compiler_error::invalid_cast;

			if (type_value->is_same_unmodified(*this_->type))
				return;//No conversion needed

			return;
		}

		if (type_value->is_vref()){
			if ((type_value->is_const() || !this_->type->is_const()) && (this_->type->is_ref() || this_->type->is_vref()) && type_value->is_same_unmodified(*this_->type))
				this_->type = type_value;
			else//Error
				throw elang::vm::compiler_error::invalid_cast;
			return;
		}

		if (type_value->is_const())
			throw elang::vm::compiler_error::invalid_cast;

		if (this_->is_constant && this_->is_static && this_->type->is_numeric()){
			if (this_->type->primitive_id() != type_value->primitive_id()){//Do conversion if types are different
				if (type_value->is_integral()){
					this_->value = integer_value_info{ boost::apply_visitor(get_int_value(), this_->value), type_value->primitive_id() };
					this_->type = elang::vm::machine::compiler.find_primitive_type(type_value->primitive_id());
				}
				else if (type_value->is_numeric()){
					this_->value = boost::apply_visitor(get_float_value(), this_->value);
					this_->type = elang::vm::machine::compiler.find_primitive_type(elang::common::primitive_type_id::float_);
				}
				else//Error
					throw elang::vm::compiler_error::invalid_cast;
			}
		}
		else if (this_->type->is_numeric() && type_value->is_numeric()){
			if (this_->type->primitive_id() != type_value->primitive_id()){
				auto left_reg = elang::vm::machine::compiler.store().get(type_value->id());
				auto right_reg = load_register::load(ELANG_AST_COMMON_TRAVERSER_OUT_DREF);

				auto lreg_op = std::make_shared<elang::easm::instruction::register_operand>(*left_reg);
				auto rreg_op = std::make_shared<elang::easm::instruction::register_operand>(*right_reg);

				auto instruction = std::make_shared<elang::easm::instruction::extended_mov>(std::vector<instruction_operand_ptr_type>{ lreg_op, rreg_op });
				elang::vm::machine::compiler.section(elang::easm::section_id::text).add(instruction);
				elang::vm::machine::compiler.store().put(*right_reg);

				this_->value = left_reg;
				this_->type = elang::vm::machine::compiler.find_primitive_type(type_value->primitive_id());
			}
			else//No conversion necessary
				this_->value = load_register::load(ELANG_AST_COMMON_TRAVERSER_OUT_DREF);
		}
		else if (type_value->is_pointer() && (this_->type->is_pointer() || this_->type->is_null_pointer())){
			if (this_->type->is_null_pointer() || this_->type->underlying_type()->is_void() || type_value->underlying_type()->is_void()){
				this_->value = load_register::load(ELANG_AST_COMMON_TRAVERSER_OUT_DREF);
				this_->type = type_value;
			}
			else//Error
				throw elang::vm::compiler_error::invalid_cast;
		}
		else//Error
			throw elang::vm::compiler_error::invalid_cast;
	}

	void operator ()(const non_operator_term &ast) const{
		boost::apply_visitor(*this, ast.value);
	}

	void operator ()(const term &ast) const{
		boost::apply_visitor(*this, ast.value);
	}

	void operator ()(const grouped_expression &ast) const{
		operator ()(ast.value);
	}

	void operator ()(const expression &ast) const{
		boost::apply_visitor(*this, ast.value);
	}

	void operator ()(const non_comma_expression &ast) const{
		boost::apply_visitor(*this, ast.value);
	}

	bool call_cast_operator(const type &target) const{
		return false;
	}

	bool call_binary_operator(elang::common::operator_id, operand_value_info &other) const{
		return false;
	}

	template <typename ast_type>
	void binary_expression(const ast_type &ast) const{
		operator ()(ast.first);//Evaluate left
		for (auto &other_ast : ast.second)//Evaluate cumulative
			boost::apply_visitor(*this, other_ast.value);
	}

	template <typename ast_type>
	void accumulate(const ast_type &ast, bool is_boolean = false) const{
		operand_value_info other;
		expression_traverser other_traverser(other);

		auto this_ = ELANG_AST_COMMON_TRAVERSER_OUT;
		other_traverser(ast.second);
		if (call_binary_operator(ast.first, other))
			return;//Operator called

		if (this_->is_constant && this_->is_static && other.is_constant && other.is_static){//Static evaluation
			if (this_->type->is_integral() && other.type->is_integral())
				static_evaluator::integral(ast.first, *this_, other, is_boolean);
			else if (this_->type->is_numeric() && other.type->is_numeric())
				static_evaluator::numeric(ast.first, *this_, other, is_boolean);
			else if (this_->type->is_pointer())
				static_evaluator::pointer(ast.first, *this_, other, is_boolean);
			else//Error
				throw elang::vm::compiler_error::invalid_operation;
		}
		else if (this_->type->is_integral() && other.type->is_integral())
			runtime_evaluator::integral(ast.first, *this_, other, is_boolean);
		else if (this_->type->is_numeric() && other.type->is_numeric())
			runtime_evaluator::numeric(ast.first, *this_, other, is_boolean);
		else if (this_->type->is_pointer())
			runtime_evaluator::pointer(ast.first, *this_, other, is_boolean);
		else//Error
			throw elang::vm::compiler_error::invalid_operation;
	}

	template <typename traverser_type, typename ast_type>
	void forward_ast(const ast_type &ast) const{
		traverser_type traverser(ELANG_AST_COMMON_TRAVERSER_OUT_DREF);
		traverser(ast);
	}
};

ELANG_AST_END

ELANG_AST_ADAPT_SINGLE(grouped_expression)

ELANG_AST_ADAPT_PAIR(static_cast_expression)
ELANG_AST_ADAPT_PAIR(reinterpret_cast_expression)
ELANG_AST_ADAPT_PAIR(dynamic_cast_expression)

ELANG_AST_ADAPT_SINGLE(cast_expression)

ELANG_AST_ADAPT_SINGLE(non_operator_term)

ELANG_AST_ADAPT_PAIR(member_access)
ELANG_AST_ADAPT_SINGLE(call)
ELANG_AST_ADAPT_SINGLE(index)

ELANG_AST_ADAPT_PAIR(right_unary_term)

ELANG_AST_ADAPT_PAIR(left_unary_term)
ELANG_AST_ADAPT_SINGLE(term)

ELANG_AST_EXPR_ADAPT_FACTOR

ELANG_AST_EXPR_ADAPT(addition)
ELANG_AST_EXPR_ADAPT(shift)

ELANG_AST_EXPR_ADAPT(relation)
ELANG_AST_EXPR_ADAPT(equality)

ELANG_AST_EXPR_ADAPT(bitwise_and)
ELANG_AST_EXPR_ADAPT(bitwise_xor)
ELANG_AST_EXPR_ADAPT(bitwise_or)

ELANG_AST_EXPR_ADAPT(logical_and)
ELANG_AST_EXPR_ADAPT(logical_or)

ELANG_AST_ADAPT_PAIR(full_right_associative)
ELANG_AST_ADAPT_SINGLE(right_associative)
ELANG_AST_ADAPT_PAIR(right_associative_expression)

ELANG_AST_ADAPT_PAIR(full_ternary)
ELANG_AST_ADAPT_SINGLE(ternary)
ELANG_AST_ADAPT_PAIR(ternary_expression)

ELANG_AST_ADAPT_PAIR(full_comma)
ELANG_AST_ADAPT_SINGLE(comma)
ELANG_AST_ADAPT_PAIR(comma_expression)

ELANG_AST_ADAPT_SINGLE(expression)
ELANG_AST_ADAPT_SINGLE(non_comma_expression)

#endif /* !ELANG_EXPRESSION_AST_H */
