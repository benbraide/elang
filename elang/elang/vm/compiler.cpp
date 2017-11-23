#include "../asm/instruction_operand/label_instruction_operand.h"
#include "../asm/instruction_operand/memory_instruction_operand.h"

#include "machine.h"
#include "compiler.h"

elang::vm::compiler::compiler()
	: states_(state_type::nil), label_count_(0), short_circuit_count_(0){
	section_map_[section_id_type::meta] = std::make_shared<elang::easm::instruction_section>(section_id_type::meta);
	section_map_[section_id_type::rodata] = std::make_shared<elang::easm::instruction_section>(section_id_type::rodata);
	section_map_[section_id_type::data] = std::make_shared<elang::easm::instruction_section>(section_id_type::data);
	section_map_[section_id_type::type] = std::make_shared<elang::easm::instruction_section>(section_id_type::type);
	section_map_[section_id_type::text] = std::make_shared<elang::easm::instruction_section>(section_id_type::text);
	section_map_[section_id_type::disabled] = std::make_shared<elang::easm::disabled_instruction_section>();

	info_ = info_type{ std::make_shared<namespace_symbol_entry>("_G", nullptr, symbol_entry_attribute::nil) };
	for (auto id = primitive_type_id_type::auto_; id <= primitive_type_id_type::float_; id = static_cast<primitive_type_id_type>(static_cast<int>(id) + 1))
		primitive_type_map_[id] = std::make_shared<primitive_type_info>(id, type_info::attribute_type::nil);
}

void elang::vm::compiler::boot(){
	store_.boot();

	states_ = state_type::nil;
	label_count_ = short_circuit_count_ = 0;

	info_.current_context = current_context_info_type{};
	info_.current_context.value = info_.global_context.get();
	info_.current_context.bubble_search = true;

	for (auto &section : section_map_)
		section.second->boot();

	section_map_.clear();
	static_const_value_map_.clear();
}

void elang::vm::compiler::begin_compilation(){
	ELANG_SET(states_, state_type::compiling);
}

void elang::vm::compiler::end_compilation(){
	ELANG_REMOVE(states_, state_type::compiling);
}

bool elang::vm::compiler::is_compiling() const{
	return ELANG_IS(states_, state_type::compiling);
}

void elang::vm::compiler::set_short_circuit(bool enabled){
	if (enabled)
		++short_circuit_count_;
	else if (short_circuit_count_ > 0u)
		--short_circuit_count_;
}

elang::vm::register_store &elang::vm::compiler::store(){
	return store_;
}

elang::vm::compiler::section_type &elang::vm::compiler::section(section_id_type id){
	if (short_circuit_count_ > 0u)//Short-circuited
		return *section_map_[section_id_type::disabled];

	auto entry = section_map_.find(id);
	if (entry != section_map_.end())
		return *entry->second;

	throw error_type::no_section;
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

std::string elang::vm::compiler::mangle(mangle_target target) const{
	switch (target){
	case mangle_target::pointer:
		return "P";
	case mangle_target::array_:
		return "A";
	case mangle_target::function_:
		return "F";
	case mangle_target::variadic:
		return "V";
	case mangle_target::const_:
		return "K";
	case mangle_target::ref_:
		return "R";
	case mangle_target::vref:
		return "U";
	case mangle_target::operator_:
		return "O";
	case mangle_target::constructor:
		return "C";
	case mangle_target::destructor:
		return "D";
	default:
		break;
	}

	throw compiler_error::unreachable;
}

void elang::vm::compiler::reset_warnings(){
	//#TODO: Implement
}

void elang::vm::compiler::add_warning(compiler_warning value){
	//#TODO: Implement
}

elang::vm::symbol_entry *elang::vm::compiler::find(const std::string &key){
	if (!info_.current_context.bubble_search)
		return info_.current_context.value->find(key);

	symbol_entry *value;
	info_.current_context.search_offset = 0u;

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
