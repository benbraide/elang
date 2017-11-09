#include "machine.h"
#include "compiler.h"

elang::vm::compiler::compiler()
	: register_list_(4), expression_type_(machine_value_type_id::unknown), label_count_(0){
	section_map_[section_id_type::meta] = std::make_shared<section_type>(section_id_type::meta);
	section_map_[section_id_type::rodata] = std::make_shared<section_type>(section_id_type::rodata);
	section_map_[section_id_type::data] = std::make_shared<section_type>(section_id_type::data);
	section_map_[section_id_type::type] = std::make_shared<section_type>(section_id_type::type);
	section_map_[section_id_type::text] = std::make_shared<section_type>(section_id_type::text);
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

unsigned int elang::vm::compiler::label_count(){
	return label_count_++;
}
