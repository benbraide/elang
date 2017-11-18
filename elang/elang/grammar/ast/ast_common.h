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

struct get_int_value{
	__int64 operator ()(__int64 value) const{
		return value;
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
	unsigned __int64 operator ()(__int64 value) const{
		return static_cast<unsigned __int64>(value);
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
	long double operator ()(__int64 value) const{
		return static_cast<long double>(value);
	}

	long double operator ()(long double value) const{
		return value;
	}

	template <typename value_type>
	long double operator ()(const value_type &value) const{
		throw elang::vm::compiler_error::unreachable;
	}
};

struct load_register{
	static elang::vm::machine_register *load(operand_value_info &info){
		auto reg = boost::apply_visitor(load_register(), info.value);
		return ((reg == nullptr) ? integral(boost::get<__int64>(info.value), info.type->id()) : reg);
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

		auto lreg_op = std::make_shared<elang::easm::instruction::register_operand>(*converted);
		auto rreg_op = std::make_shared<elang::easm::instruction::register_operand>(reg);

		auto instruction = std::make_shared<elang::easm::instruction::extended_mov>(std::vector<instruction_operand_ptr_type>{ lreg_op, rreg_op });
		elang::vm::machine::compiler.section(elang::easm::section_id::text).add(instruction);

		return converted;
	}

	static elang::vm::machine_register *integral(__int64 value, elang::vm::machine_value_type_id value_type){
		auto reg = get_register(value_type);
		auto reg_op = std::make_shared<elang::easm::instruction::register_operand>(*reg);

		auto const_op = std::make_shared<elang::easm::instruction::constant_value_operand<__int64>>(value);
		auto instruction = std::make_shared<elang::easm::instruction::mov>(std::vector<instruction_operand_ptr_type>{ reg_op, const_op });

		elang::vm::machine::compiler.section(elang::easm::section_id::text).add(instruction);
		return reg;
	}

	elang::vm::machine_register *operator ()(elang::common::constant_value value) const{
		elang::vm::machine_value_type_id value_type;
		switch (value){
		case elang::common::constant_value::false_:
		case elang::common::constant_value::true_:
		case elang::common::constant_value::indeterminate:
			value_type = elang::vm::machine_value_type_id::byte;
			break;
		case elang::common::constant_value::nullptr_:
			value_type = elang::vm::machine_value_type_id::qword;
			break;
		default:
			throw elang::vm::compiler_error::unreachable;
			break;
		}

		auto reg = get_register(value_type);
		auto reg_op = std::make_shared<elang::easm::instruction::register_operand>(*reg);

		auto label_op = elang::vm::machine::compiler.get_constant_operand(value);
		auto instruction = std::make_shared<elang::easm::instruction::mov>(std::vector<instruction_operand_ptr_type>{ reg_op, label_op });

		elang::vm::machine::compiler.section(elang::easm::section_id::text).add(instruction);
		return reg;
	}

	elang::vm::machine_register *operator ()(__int64 value) const{
		return nullptr;
	}

	elang::vm::machine_register *operator ()(long double value) const{
		auto reg = get_register(elang::vm::machine_value_type_id::float_);
		auto reg_op = std::make_shared<elang::easm::instruction::register_operand>(*reg);

		auto const_op = std::make_shared<elang::easm::instruction::constant_value_operand<long double>>(value);
		auto instruction = std::make_shared<elang::easm::instruction::mov>(std::vector<instruction_operand_ptr_type>{ reg_op, const_op });

		elang::vm::machine::compiler.section(elang::easm::section_id::text).add(instruction);
		return reg;
	}

	elang::vm::machine_register *operator ()(const string_operand_value_info &value) const{
		auto reg = get_register(elang::vm::machine_value_type_id::qword);
		auto reg_op = std::make_shared<elang::easm::instruction::register_operand>(*reg);

		auto label_op = std::make_shared<elang::easm::instruction::label_operand>(value.value);
		auto instruction = std::make_shared<elang::easm::instruction::mov>(std::vector<instruction_operand_ptr_type>{ reg_op, label_op });

		elang::vm::machine::compiler.section(elang::easm::section_id::text).add(instruction);
		return reg;
	}

	elang::vm::machine_register *operator ()(elang::vm::symbol_entry *value) const{
		if (ELANG_IS(value->attributes(), elang::vm::symbol_entry_attribute::static_)){//Static
			auto reg = get_register(elang::vm::machine_value_type_id::qword);
			auto reg_op = std::make_shared<elang::easm::instruction::register_operand>(*reg);

			auto label_op = std::make_shared<elang::easm::instruction::label_operand>(value->mangle());
			auto instruction = std::make_shared<elang::easm::instruction::mov>(std::vector<instruction_operand_ptr_type>{ reg_op, label_op });

			elang::vm::machine::compiler.section(elang::easm::section_id::text).add(instruction);
			return reg;
		}

		throw elang::vm::compiler_error::unreachable;
	}

	elang::vm::machine_register *operator ()(elang::vm::machine_register *value) const{
		return value;
	}

	elang::vm::machine_register *operator ()(elang::easm::instruction::operand_base::ptr_type value) const{
		auto reg = get_register(value->value_type());
		auto reg_op = std::make_shared<elang::easm::instruction::register_operand>(*reg);

		auto instruction = std::make_shared<elang::easm::instruction::mov>(std::vector<instruction_operand_ptr_type>{ reg_op, value });
		elang::vm::machine::compiler.section(elang::easm::section_id::text).add(instruction);

		return reg;
	}
};

ELANG_AST_END

#endif /* !ELANG_AST_COMMON_H */
