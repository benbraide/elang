#include "../asm/instruction_operand/label_instruction_operand.h"
#include "asm_translation.h"

elang::vm::asm_translation::asm_translation()
	: active_label_(nullptr), active_section_(nullptr){
	section_order_.reserve(4);
	section_order_.push_back(section_id_type::rodata);
	section_order_.push_back(section_id_type::data);
	section_order_.push_back(section_id_type::type);
	section_order_.push_back(section_id_type::text);
}

void elang::vm::asm_translation::add(section_id_type id){
	if (section_map_.find(id) == section_map_.end())
		active_section_ = &section_map_.try_emplace(id, id).first->second;
	else//Error
		throw error_type::section_redifinition;
}

void elang::vm::asm_translation::add(uint_type nested_level, const std::string &label){
	if (active_section_ == nullptr)
		throw error_type::no_section;

	if (active_label_ != nullptr){
		if (nested_level <= active_label_->nested_level()){//Find parent
			auto parent = active_label_->parent(active_label_->nested_level() - nested_level);
			if (parent == nullptr)//Add to section
				active_section_->add(nullptr, label);
			else//Use parent
				parent->add(label);
		}
		else//Active is parent
			active_label_->add(label);
	}
	else//First label
		active_section_->add(nullptr, label);
}

void elang::vm::asm_translation::add(label_type &label){
	active_label_ = &label;
}

void elang::vm::asm_translation::add(label_operand_type &label_op){
	label_operand_list_.push_back(&label_op);
}

elang::vm::asm_translation::section_type *elang::vm::asm_translation::active_section() const{
	return active_section_;
}

elang::vm::asm_translation::label_type *elang::vm::asm_translation::active_label() const{
	return active_label_;
}

void elang::vm::asm_translation::bundle(){
	uint64_type seg_offset = (elang::vm::machine::access_protected.to + 1u);
	for (auto id : section_order_){
		auto entry = section_map_.find(id);
		if (entry != section_map_.end()){//Update 'seg_offset'
			entry->second.set_seg_offset(seg_offset);
			seg_offset += entry->second.get_offset();
		}
	}

	for (auto entry : label_operand_list_)
		entry->resolve(*this);//Resolve unresolved labels
}

elang::vm::asm_translation::label_type *elang::vm::asm_translation::find(const std::string &first, const std::vector<std::string> &rest) const{
	label_type *label = nullptr;
	for (auto &entry : section_map_){
		if ((label = entry.second.find(first, rest)) != nullptr)
			return label;
	}

	return nullptr;
}

elang::vm::asm_translation::uint64_type elang::vm::asm_translation::find(const label_type &label) const{
	uint64_type value;
	for (auto &entry : section_map_){
		if ((value = entry.second.find(label)) != static_cast<uint64_type>(-1))
			return value;
	}

	return static_cast<uint64_type>(-1);
}

elang::vm::asm_translation::uint64_type elang::vm::asm_translation::find_address(const std::string &first, const std::vector<std::string> &rest) const{
	uint64_type value;
	for (auto &entry : section_map_){
		if ((value = entry.second.find_address(first, rest)) != static_cast<uint64_type>(-1))
			return value;
	}

	return static_cast<uint64_type>(-1);
}

void elang::vm::asm_translation::print(writer_type &writer, writer_type &wide_writer) const{
	for (auto id : section_order_){
		auto entry = section_map_.find(id);
		if (entry != section_map_.end())
			entry->second.print(writer, wide_writer);
	}
}
