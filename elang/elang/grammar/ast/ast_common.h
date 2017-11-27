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

using instruction_operand_ptr_type = elang::easm::instruction::operand_base::ptr_type;
using instruction_ptr_type = elang::easm::instruction::base::ptr_type;

struct integer_value_info{
	__int64 value;
	elang::common::primitive_type_id type;
};

struct string_operand_value_info{
	std::string value;
	bool is_wide;
};

struct memory_operand_value_info{
	std::string label;
	std::size_t offset;
	std::size_t size;
};

using operand_value_type = boost::variant<
	elang::common::constant_value,
	integer_value_info,
	long double,
	string_operand_value_info,
	memory_operand_value_info,
	elang::vm::symbol_entry *,
	elang::vm::machine_register *,
	instruction_operand_ptr_type
>;

struct operand_value_info{
	operand_value_type value;
	elang::vm::type_info::ptr_type type;
	bool is_constant;
	bool is_static;
};

struct get_int_value{
	__int64 operator ()(elang::common::constant_value value) const{
		switch (value){
		case elang::common::constant_value::false_:
			return 0;
		case elang::common::constant_value::true_:
			return 1;
		case elang::common::constant_value::indeterminate:
			return -1;
		case elang::common::constant_value::nullptr_:
			return 0;
		default:
			throw elang::vm::compiler_error::unreachable;
			break;
		}

		return 0;
		
	}

	__int64 operator ()(const integer_value_info &info) const{
		return info.value;
	}

	__int64 operator ()(long double value) const{
		return static_cast<__int64>(value);
	}

	template <typename value_type>
	__int64 operator ()(const value_type &value) const{
		throw elang::vm::compiler_error::unreachable;
	}
};

struct get_uint_value{
	unsigned __int64 operator ()(const integer_value_info &info) const{
		return static_cast<unsigned __int64>(info.value);
	}

	unsigned __int64 operator ()(long double value) const{
		return static_cast<unsigned __int64>(value);
	}

	template <typename value_type>
	unsigned __int64 operator ()(const value_type &value) const{
		throw elang::vm::compiler_error::unreachable;
	}
};

struct get_float_value{
	long double operator ()(const integer_value_info &info) const{
		return static_cast<long double>(info.value);
	}

	long double operator ()(long double value) const{
		return value;
	}

	template <typename value_type>
	long double operator ()(const value_type &value) const{
		throw elang::vm::compiler_error::unreachable;
	}
};

struct operand_creator{
	using variant_type = boost::variant<
		elang::vm::machine_value_type_id,
		elang::vm::machine_register *,
		std::string,
		__int64,
		long double,
		instruction_operand_ptr_type
	>;

	struct memory_type{
		variant_type value;
	};

	struct memory_expr_type{
		std::vector<variant_type> value;
		bool add;
	};

	template <typename... args_types>
	static instruction_operand_ptr_type create_memory(args_types &&... args){
		return std::make_shared<elang::easm::instruction::memory_operand>(elang::vm::machine_value_type_id::unknown, create(std::forward<value_type>(args)...));
	}

	static instruction_operand_ptr_type create(elang::vm::machine_value_type_id value_type){
		auto reg = elang::vm::machine::compiler.store().get(value_type);
		if (reg == nullptr)//Out of registers
			throw elang::vm::machine_error::no_register;
		return std::make_shared<elang::easm::instruction::register_operand>(*reg);
	}

	static instruction_operand_ptr_type create(elang::vm::machine_register &reg){
		return std::make_shared<elang::easm::instruction::register_operand>(reg);
	}

	static instruction_operand_ptr_type create(const std::string &label){
		return std::make_shared<elang::easm::instruction::label_operand>(label);
	}

	static instruction_operand_ptr_type create(__int64 value){
		return std::make_shared<elang::easm::instruction::constant_value_operand<__int64>>(value);
	}

	static instruction_operand_ptr_type create(long double value){
		return std::make_shared<elang::easm::instruction::constant_value_operand<long double>>(value);
	}

	static instruction_operand_ptr_type create(const memory_type &info){
		return std::make_shared<elang::easm::instruction::memory_operand>(elang::vm::machine_value_type_id::unknown, create(info.value));
	}

	static instruction_operand_ptr_type create(const memory_expr_type &info){
		return std::make_shared<elang::easm::instruction::memory_operand>(elang::vm::machine_value_type_id::unknown, create(info.value, info.add));
	}

	static instruction_operand_ptr_type create(const std::vector<variant_type> &values, bool add = true){
		if (values.empty())//Error
			throw elang::vm::compiler_error::unreachable;

		auto op = (add ? elang::easm::instruction_operator_id::add : elang::easm::instruction_operator_id::sub);
		instruction_operand_ptr_type lop = boost::apply_visitor(operand_creator(), values[0]), rop;

		for (auto iter = std::next(values.begin()); iter != values.end(); ++iter){//Create expressions
			rop = boost::apply_visitor(operand_creator(), *iter);
			lop = std::make_shared<elang::easm::instruction::expression_operand>(op, lop, rop);
		}

		return lop;
	}

	static instruction_operand_ptr_type create(instruction_operand_ptr_type value){
		return value;
	}

	static instruction_operand_ptr_type create(const variant_type &info){
		return boost::apply_visitor(operand_creator(), info);
	}

	instruction_operand_ptr_type operator ()(elang::vm::machine_value_type_id value_type) const{
		return create(value_type);
	}

	instruction_operand_ptr_type operator ()(elang::vm::machine_register *reg) const{
		return create(*reg);
	}

	instruction_operand_ptr_type operator ()(const std::string &label) const{
		return create(label);
	}

	instruction_operand_ptr_type operator ()(__int64 value) const{
		return create(value);
	}

	instruction_operand_ptr_type operator ()(long double value) const{
		return create(value);
	}

	instruction_operand_ptr_type operator ()(instruction_operand_ptr_type value) const{
		return value;
	}
};

struct instruction_creator{
	template <typename instruction_type>
	static instruction_ptr_type create(){
		return std::make_shared<instruction_type>(std::vector<instruction_operand_ptr_type>{});
	}

	template <typename instruction_type>
	static instruction_ptr_type create(instruction_operand_ptr_type lhs){
		return std::make_shared<instruction_type>(std::vector<instruction_operand_ptr_type>{ lhs });
	}

	template <typename instruction_type>
	static instruction_ptr_type create(instruction_operand_ptr_type lhs, instruction_operand_ptr_type rhs){
		return std::make_shared<instruction_type>(std::vector<instruction_operand_ptr_type>{ lhs, rhs });
	}

	template <typename instruction_type, typename... operands_types>
	static instruction_ptr_type create(operands_types &&... operands){
		std::vector<instruction_operand_ptr_type> list;
		stack_operands(list, std::forward<operands_types>(operands)...);
		return std::make_shared<instruction_type>(list);
	}

	template <typename first_type, typename... operands_types>
	static void stack_operands(std::vector<instruction_operand_ptr_type> &list, const first_type &op, operands_types &&... operands){
		list.push_back(operand_creator::create(op));
		stack_operands(list, std::forward<operands_types>(operands)...);
	}

	template <typename first_type, typename... operands_types>
	static void stack_operands(std::vector<instruction_operand_ptr_type> &list, first_type &op, operands_types &&... operands){
		list.push_back(operand_creator::create(op));
		stack_operands(list, std::forward<operands_types>(operands)...);
	}

	static void stack_operands(std::vector<instruction_operand_ptr_type> &list){}

	static void add_to_section(elang::easm::section_id id, instruction_ptr_type value){
		elang::vm::machine::compiler.section(id).add(value);
	}

	static void add_to_text_section(instruction_ptr_type value){
		elang::vm::machine::compiler.section(elang::easm::section_id::text).add(value);
	}

	template <typename instruction_type, typename... operands_types>
	static void add_to_text_section(operands_types &&... operands){
		add_to_text_section(create<instruction_type>(std::forward<operands_types>(operands)...));
	}
};

struct load_register{
	static elang::vm::machine_register *load(operand_value_info &info){
		return boost::apply_visitor(load_register(), info.value);
	}

	static elang::vm::machine_register *get_register(elang::vm::machine_value_type_id value_type){
		auto reg = elang::vm::machine::compiler.store().get(value_type);
		if (reg == nullptr)//Out of registers
			throw elang::vm::machine_error::no_register;
		return reg;
	}

	static elang::vm::machine_register *convert_register(elang::vm::machine_register &reg, elang::vm::machine_value_type_id value_type){
		auto converted = elang::vm::machine::compiler.store().convert(reg, value_type);
		if (converted == nullptr)//Error
			throw elang::vm::machine_error::no_register;

		if (converted == &reg)//No conversion necessary
			return converted;

		instruction_creator::add_to_text_section<elang::easm::instruction::extended_mov>(reg, *converted);
		return converted;
	}

	static elang::vm::machine_register *integral(__int64 value, elang::vm::machine_value_type_id value_type){
		auto reg = get_register(value_type);
		instruction_creator::add_to_text_section<elang::easm::instruction::mov>(*reg, value);
		return reg;
	}

	elang::vm::machine_register *operator ()(elang::common::constant_value value) const{
		__int64 intern_val;
		elang::vm::machine_value_type_id value_type;

		switch (value){
		case elang::common::constant_value::false_:
			intern_val = 0;
			value_type = elang::vm::machine_value_type_id::byte;
			break;
		case elang::common::constant_value::true_:
			intern_val = 1;
			value_type = elang::vm::machine_value_type_id::byte;
			break;
		case elang::common::constant_value::indeterminate:
			intern_val = -1;
			value_type = elang::vm::machine_value_type_id::byte;
			break;
		case elang::common::constant_value::nullptr_:
			intern_val = 0;
			value_type = elang::vm::machine_value_type_id::qword;
			break;
		default:
			throw elang::vm::compiler_error::unreachable;
			break;
		}

		auto reg = get_register(value_type);
		instruction_creator::add_to_text_section<elang::easm::instruction::mov>(*reg, intern_val);
		return reg;
	}

	elang::vm::machine_register *operator ()(const integer_value_info &info) const{
		if (info.type == elang::common::primitive_type_id::pointer)//Pointer value
			return integral(info.value, elang::vm::machine_value_type_id::qword);

		switch (info.type){
		case elang::common::primitive_type_id::char_:
		case elang::common::primitive_type_id::int8_:
		case elang::common::primitive_type_id::uint8_:
			return integral(info.value, elang::vm::machine_value_type_id::byte);
		case elang::common::primitive_type_id::wchar_:
		case elang::common::primitive_type_id::int16_:
		case elang::common::primitive_type_id::uint16_:
			return integral(info.value, elang::vm::machine_value_type_id::word);
		case elang::common::primitive_type_id::int32_:
		case elang::common::primitive_type_id::uint32_:
			return integral(info.value, elang::vm::machine_value_type_id::dword);
		case elang::common::primitive_type_id::pointer:
		case elang::common::primitive_type_id::int64_:
		case elang::common::primitive_type_id::uint64_:
			return integral(info.value, elang::vm::machine_value_type_id::qword);
		default:
			throw elang::vm::compiler_error::unreachable;
			break;
		}

		return nullptr;
	}

	elang::vm::machine_register *operator ()(long double value) const{
		auto reg = get_register(elang::vm::machine_value_type_id::float_);
		instruction_creator::add_to_text_section<elang::easm::instruction::mov>(*reg, value);
		return reg;
	}

	elang::vm::machine_register *operator ()(const string_operand_value_info &info) const{
		auto reg = get_register(elang::vm::machine_value_type_id::qword);
		instruction_creator::add_to_text_section<elang::easm::instruction::mov>(*reg, info.value);
		return reg;
	}

	elang::vm::machine_register *operator ()(const memory_operand_value_info &info) const{
		auto reg = get_register(elang::vm::machine_value_type_id::qword);
		auto &search_label = elang::vm::machine::compiler.info().current_context.search_label;

		instruction_operand_ptr_type base_op;
		if (!info.label.empty())
			base_op = operand_creator::create(info.label);
		else if (!search_label.empty())//Offset from static start
			base_op = operand_creator::create(search_label);
		else//Local storage
			base_op = operand_creator::create(*elang::vm::machine::cached_registers.base_pointer);

		if (info.offset > 0u){//Add offset
			operand_creator::memory_expr_type expr{
				{ base_op, static_cast<__int64>(info.offset) },	//Expression operands
				true											//Perform addition
			};

			instruction_creator::add_to_text_section<elang::easm::instruction::mov>(*reg, expr);
		}
		else//No offset
			instruction_creator::add_to_text_section<elang::easm::instruction::mov>(*reg, operand_creator::memory_type{ base_op });

		return reg;
	}

	elang::vm::machine_register *operator ()(elang::vm::symbol_entry *value) const{
		throw elang::vm::compiler_error::variable_expected;
	}

	elang::vm::machine_register *operator ()(elang::vm::machine_register *value) const{
		return value;
	}

	elang::vm::machine_register *operator ()(instruction_operand_ptr_type value) const{
		auto reg = get_register(value->value_type());
		instruction_creator::add_to_text_section<elang::easm::instruction::mov>(*reg, value);
		return reg;
	}
};

ELANG_AST_END

#endif /* !ELANG_AST_COMMON_H */
