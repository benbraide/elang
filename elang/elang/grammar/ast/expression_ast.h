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

struct is_constant{
	template <typename ast_type>
	bool operator ()(const ast_type &ast) const{
		return false;
	}

	bool operator ()(const constant_value &ast) const{
		return true;
	}

	bool operator ()(const literal_value &ast) const{
		return true;
	}

	bool operator ()(const identifier_compatible &ast) const{
		auto symbol = identifier_resolver()(ast);
		return true;
	}

	bool operator ()(const non_operator_term &ast) const{
		return boost::apply_visitor(is_constant(), ast.value);
	}

	bool operator ()(const left_unary_term &ast) const{
		if (ast.first != elang::common::operator_id::plus && ast.first != elang::common::operator_id::minus)
			return false;
		return boost::apply_visitor(is_constant(), ast.second);
	}

	bool operator ()(const term &ast) const{
		return boost::apply_visitor(is_constant(), ast.value);
	}
};

struct right_unary_term_traverser{

};

struct expression_traverser{
	ELANG_AST_COMMON_TRAVERSER_BEGIN(expression_traverser)

	template <typename ast_type>
	void operator ()(const ast_type &ast) const{
		
	}

	void operator ()(const constant_value &ast) const{
		constant_value_traverser traverser(ELANG_AST_COMMON_TRAVERSER_OUT_DREF);
		traverser(ast);
	}

	void operator ()(const literal_value &ast) const{
		literal_value_traverser traverser(ELANG_AST_COMMON_TRAVERSER_OUT_DREF);
		traverser(ast);
	}

	void operator ()(const identifier_compatible &ast) const{
		identifier_traverser traverser(ELANG_AST_COMMON_TRAVERSER_OUT_DREF);
		traverser(ast);
	}

	void operator ()(const factor_expression &ast) const{
		operand_value_info other;
		expression_traverser traverser(ELANG_AST_COMMON_TRAVERSER_OUT_DREF), other_traverser(other);

		traverser(ast.first);//Evaluate left
		auto this_ = ELANG_AST_COMMON_TRAVERSER_OUT;

		for (auto &other_ast : ast.second){//Evaluate cumulative
			other_traverser(other_ast.second);
			if (call_binary_operator(other_ast.first, other))
				continue;//Operator called

			if (this_->is_constant && this_->is_static && other.is_constant && other.is_static && this_->type->is_numeric() && other.type->is_numeric()){//Static evaluation
				if (this_->type->is_integral() && other.type->is_integral()){//Integral evaluation
					this_->value = (boost::apply_visitor(get_int_value(), this_->value) * boost::apply_visitor(get_int_value(), other.value));
					this_->type = ((this_->type->primitive_id() < other.type->primitive_id()) ? other.type : this_->type);
				}
				else{//Float evaluation
					this_->value = (boost::apply_visitor(get_float_value(), this_->value) * boost::apply_visitor(get_float_value(), other.value));
					this_->type = elang::vm::machine::compiler.find_primitive_type(elang::common::primitive_type_id::float_);
				}
			}
			else if (this_->type->is_numeric() && other.type->is_numeric()){//Generate instruction
				auto left_reg = load_register::load(*this_);
				auto right_reg = load_register::load(other);

				if (left_reg->type_id() < right_reg->type_id())//Type conversion
					left_reg = load_register::convert_register(*left_reg, right_reg->type_id());
				else if (right_reg->type_id() < left_reg->type_id())//Type conversion
					right_reg = load_register::convert_register(*right_reg, left_reg->type_id());

				auto left_reg_op = std::make_shared<elang::easm::instruction::register_operand>(*left_reg);
				auto right_reg_op = std::make_shared<elang::easm::instruction::register_operand>(*right_reg);
				auto instruction = std::make_shared<elang::easm::instruction::mult>(std::vector<instruction_operand_ptr_type>{ left_reg_op, right_reg_op });

				elang::vm::machine::compiler.section(elang::easm::section_id::text).add(instruction);
				elang::vm::machine::compiler.store().put(*right_reg);

				this_->value = left_reg;//Update value
				this_->is_constant = this_->is_static = false;//Reset
			}
			else//Error
				throw elang::vm::compiler_error::invalid_operation;

			other.is_constant = other.is_static = false;//Reset
		}
	}

	void operator ()(const static_cast_expression &ast) const{
		
	}

	void operator ()(const non_operator_term &ast) const{
		boost::apply_visitor(expression_traverser(ELANG_AST_COMMON_TRAVERSER_OUT_DREF), ast.value);
	}

	void operator ()(const term &ast) const{
		boost::apply_visitor(expression_traverser(ELANG_AST_COMMON_TRAVERSER_OUT_DREF), ast.value);
	}

	void operator ()(const grouped_expression &ast) const{
		operator ()(ast.value);
	}

	void operator ()(const expression &ast) const{
		boost::apply_visitor(expression_traverser(ELANG_AST_COMMON_TRAVERSER_OUT_DREF), ast.value);
	}

	void operator ()(const non_comma_expression &ast) const{
		boost::apply_visitor(expression_traverser(ELANG_AST_COMMON_TRAVERSER_OUT_DREF), ast.value);
	}

	bool valid_binary_operation(elang::common::operator_id, operand_value_info &other) const{
		return true;
	}

	bool call_binary_operator(elang::common::operator_id, operand_value_info &other) const{
		return false;
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
