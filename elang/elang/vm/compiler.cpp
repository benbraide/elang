#include "../asm/instruction_operand/label_instruction_operand.h"
#include "../asm/instruction_operand/memory_instruction_operand.h"

#include "machine.h"
#include "compiler.h"

elang::vm::compiler::compiler()
	: register_list_(4), expression_type_(machine_value_type_id::unknown), label_count_(0){
	section_map_[section_id_type::meta] = std::make_shared<elang::easm::instruction_section>(section_id_type::meta);
	section_map_[section_id_type::rodata] = std::make_shared<elang::easm::instruction_section>(section_id_type::rodata);
	section_map_[section_id_type::data] = std::make_shared<elang::easm::instruction_section>(section_id_type::data);
	section_map_[section_id_type::type] = std::make_shared<elang::easm::instruction_section>(section_id_type::type);
	section_map_[section_id_type::text] = std::make_shared<elang::easm::instruction_section>(section_id_type::text);

	info_ = info_type{ std::make_shared<namespace_symbol_entry>("_G", nullptr, symbol_entry_attribute::nil) };
	info_.current_context.value = info_.global_context.get();
	info_.current_context.bubble_search = true;

	for (auto id = primitive_type_id_type::auto_; id <= primitive_type_id_type::float_; id = static_cast<primitive_type_id_type>(static_cast<int>(id) + 1))
		primitive_type_map_[id] = std::make_shared<primitive_type_info>(id, type_info::attribute_type::nil);
}

bool elang::vm::compiler::is_compiling() const{
	return false;
}

elang::vm::register_store &elang::vm::compiler::store(){
	return store_;
}

elang::vm::compiler::section_type &elang::vm::compiler::section(section_id_type id){
	auto entry = section_map_.find(id);
	if (entry != section_map_.end())
		return *entry->second;

	throw error_type::no_section;
}

void elang::vm::compiler::push_register(machine_register &reg){
	for (auto entry = register_list_.begin(); entry != register_list_.end(); ++entry){
		if (*entry == nullptr){
			*entry = &reg;
			return;
		}
	}

	throw machine_error::no_register;
}

elang::vm::machine_register *elang::vm::compiler::pop_register(){
	if (register_list_[0] == nullptr)
		return nullptr;

	auto reg = register_list_[0];
	auto entry = register_list_.begin();

	for (auto next = std::next(entry); next != register_list_.end(); ++entry, ++next){
		if (*entry != nullptr)
			*entry = *next;
		else//No used spot
			break;
	}

	return reg;
}

void elang::vm::compiler::reset_expression_type(){
	expression_type_ = machine_value_type_id::unknown;
}

void elang::vm::compiler::set_expression_type(machine_value_type_id left, machine_value_type_id right){
	auto type = ((left < right) ? right : left);
	if (expression_type_ == machine_value_type_id::unknown || expression_type_ < type)
		expression_type_ = type;
}

elang::vm::machine_value_type_id elang::vm::compiler::get_expression_type() const{
	return expression_type_;
}

std::string elang::vm::compiler::generate_label(label_type type){
	switch (type){
	case label_type::constant:
		return ("__LC" + std::to_string(label_count_++) + "__");
	default:
		break;
	}

	throw compiler_error::unreachable;
}

elang::vm::compiler::instruction_operand_ptr_type elang::vm::compiler::get_constant_operand(elang::common::constant_value type){
	auto entry = constant_value_map_.find(type);
	if (entry != constant_value_map_.end())
		return entry->second;

	std::string label;
	machine_value_type_id size;

	switch (type){
	case elang::common::constant_value::false_:
		label = "__false__";
		size = machine_value_type_id::byte;
		break;
	case elang::common::constant_value::true_:
		label = "__true__";
		size = machine_value_type_id::byte;
		break;
	case elang::common::constant_value::indeterminate:
		label = "__ind__";
		size = machine_value_type_id::byte;
		break;
	case elang::common::constant_value::nullptr_:
		label = "__null__";
		size = machine_value_type_id::qword;
		break;
	case elang::common::constant_value::nan_:
		label = "__nan__";
		size = machine_value_type_id::qword;
		break;
	case elang::common::constant_value::infinite_:
		label = "__inf__";
		size = machine_value_type_id::qword;
		break;
	default:
		throw compiler_error::unreachable;
		break;
	}

	auto label_op = std::make_shared<elang::easm::instruction::label_operand>(label, std::vector<std::string>());
	return (constant_value_map_[type] = std::make_shared<elang::easm::instruction::memory_operand>(size, label_op));
}

void elang::vm::compiler::reset_warnings(){
	//#TODO: Implement
}

void elang::vm::compiler::add_warning(compiler_warning value){
	//#TODO: Implement
}

elang::vm::symbol_entry *elang::vm::compiler::find(const std::string &key) const{
	if (!info_.current_context.bubble_search)
		return info_.current_context.value->find(key);

	symbol_entry *value;
	for (auto context = info_.current_context.value; context != nullptr; context = dynamic_cast<storage_symbol_entry *>(context->parent())){
		if ((value = context->find(key)) != nullptr)
			return value;
	}

	return nullptr;
}

elang::vm::compiler::type_info_ptr_type elang::vm::compiler::find_primitive_type(primitive_type_id_type id) const{
	auto entry = primitive_type_map_.find(id);
	return ((entry == primitive_type_map_.end()) ? nullptr : entry->second);
}

elang::vm::compiler::info_type &elang::vm::compiler::info(){
	return info_;
}
