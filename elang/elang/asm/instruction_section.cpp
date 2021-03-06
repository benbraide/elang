#include "../vm/machine.h"

#include "instruction/times_instruction.h"

#include "instruction_section.h"

elang::easm::instruction_section_base::instruction_section_base(id_type id)
	: id_(id), offset_(0), seg_offset_(0){}

void elang::easm::instruction_section_base::boot(){
	offset_ = seg_offset_ = 0;
}

elang::easm::instruction_section_base::id_type elang::easm::instruction_section_base::id() const{
	return id_;
}

void elang::easm::instruction_section_base::print(writer_type &writer) const{
	writer.write("section .");
	switch (id_){
	case id_type::rodata:
		writer.write("rodata").write(writer_type::manip_type::newline);
		break;
	case id_type::data:
		writer.write("data").write(writer_type::manip_type::newline);
		break;
	case id_type::text:
		writer.write("text").write(writer_type::manip_type::newline);
		break;
	case id_type::type:
		writer.write("type").write(writer_type::manip_type::newline);
		break;
	case id_type::meta:
		writer.write("meta").write(writer_type::manip_type::newline);
		break;
	default:
		break;
	}

	print_content_(writer);
}

void elang::easm::instruction_section_base::set_seg_offset(uint64_type value){
	seg_offset_ = value;
	if (id() == id_type::rodata)//Protect region from write
		elang::vm::machine::memory_manager.protect_from_write(elang::memory::manager::range_type{ seg_offset_, (seg_offset_ + (offset_ - 1)) });
}

elang::easm::instruction_section_base::uint64_type elang::easm::instruction_section_base::get_seg_offset() const{
	return seg_offset_;
}

elang::easm::instruction_section_base::uint64_type elang::easm::instruction_section_base::get_offset() const{
	return offset_;
}

elang::easm::instruction_label *elang::easm::instruction_section_base::add(instruction_label *parent, const std::string &label){
	throw error_type::bad_instruction;
}

void elang::easm::instruction_section_base::add(instruction_ptr_type instruction){
	throw error_type::bad_instruction;
}

void elang::easm::instruction_section_base::add(runtime_ptr_type type){
	throw error_type::bad_instruction;
}

void elang::easm::instruction_section_base::add(const std::string &start_label){
	throw error_type::bad_instruction;
}

void elang::easm::instruction_section_base::add(size_type stack_size){
	throw error_type::bad_instruction;
}

elang::easm::instruction_section_base::instruction_type *elang::easm::instruction_section_base::find(uint64_type offset, bool is_relative) const{
	return (is_relative ? find_(offset) : find_(offset - seg_offset_));
}

elang::easm::instruction_label *elang::easm::instruction_section_base::find(const std::string &first, const std::vector<std::string> &rest) const{
	throw error_type::bad_operation;
}

elang::easm::instruction_section_base::uint64_type elang::easm::instruction_section_base::find(const instruction_label &label) const{
	throw error_type::bad_operation;
}

elang::easm::instruction_section_base::uint64_type elang::easm::instruction_section_base::find_address(const std::string &first, const std::vector<std::string> &rest) const{
	throw error_type::bad_operation;
}

void elang::easm::instruction_section_base::print_content_(writer_type &writer) const{}

elang::easm::instruction_section_base::instruction_type *elang::easm::instruction_section_base::find_(uint64_type offset) const{
	throw error_type::bad_operation;
}

elang::easm::instruction_section::instruction_section(id_type id)
	: instruction_section_base(id){}

void elang::easm::instruction_section::boot(){
	instruction_section_base::boot();

	order_list_.clear();
	instruction_list_.clear();
	label_list_.clear();
	range_map_.clear();
}

void elang::easm::instruction_section::write_memory() const{
	auto seg_offset = seg_offset_;
	for (auto &entry : order_list_){//Write instructions to memory
		if (std::holds_alternative<instruction_ptr_type>(entry))
			std::get<instruction_ptr_type>(entry)->write_memory(seg_offset, nullptr);
	}
}

elang::easm::instruction_label *elang::easm::instruction_section::add(instruction_label *parent, const std::string &label){
	auto entry = std::make_shared<instruction_label>(parent, label);

	label_list_[entry.get()] = offset_;
	order_list_.push_back(entry);

	return entry.get();
}

void elang::easm::instruction_section::add(instruction_ptr_type instruction){
	instruction->apply_required_value_type();
	instruction->validate_operands();

	auto instruction_bytes = instruction->instruction_bytes();
	if (dynamic_cast<instruction::times *>(instruction.get()) == nullptr)
		instruction_list_[offset_] = instruction;
	else//Repeat instruction
		range_map_[instruction.get()] = range_type{ offset_, (offset_ + instruction_bytes) };

	order_list_.push_back(instruction);
	offset_ += instruction_bytes;
}

elang::easm::instruction_label *elang::easm::instruction_section::find(const std::string &first, const std::vector<std::string> &rest) const{
	instruction_label *label = nullptr;
	for (auto &entry : label_list_){
		if ((label = entry.first->find(first, rest)) != nullptr)
			return label;
	}

	return nullptr;
}

elang::easm::instruction_section_base::uint64_type elang::easm::instruction_section::find(const instruction_label &label) const{
	auto entry = label_list_.find(const_cast<instruction_label *>(&label));
	return ((entry == label_list_.end()) ? static_cast<uint64_type>(-1) : (entry->second + seg_offset_));
}

elang::easm::instruction_section_base::uint64_type elang::easm::instruction_section::find_address(const std::string &first, const std::vector<std::string> &rest) const{
	for (auto &entry : label_list_){
		if (entry.first->find(first, rest) != nullptr)
			return (entry.second + seg_offset_);
	}

	return static_cast<uint64_type>(-1);
}

void elang::easm::instruction_section::print_content_(writer_type &writer) const{
	printer printer_inst(writer);
	for (auto &entry : order_list_)//Print entries
		std::visit(printer_inst, entry);
}

elang::easm::instruction_section_base::instruction_type *elang::easm::instruction_section::find_(uint64_type offset) const{
	auto entry = instruction_list_.find(offset);
	if (entry != instruction_list_.end())//Entry exists
		return entry->second.get();

	for (auto &range : range_map_){//Search ranges
		if (offset < range.second.from || offset >= range.second.to)//Outside range
			continue;

		auto value = dynamic_cast<instruction::times *>(range.first)->value();
		if (((offset - range.second.from) % value->instruction_bytes()) != 0u)
			break;//Not byte aligned

		return value.get();
	}

	return nullptr;
}

elang::easm::disabled_instruction_section::disabled_instruction_section()
	: instruction_section_base(id_type::data){}

void elang::easm::disabled_instruction_section::write_memory() const{}

elang::easm::instruction_label *elang::easm::disabled_instruction_section::add(instruction_label *parent, const std::string &label){
	return nullptr;
}

void elang::easm::disabled_instruction_section::add(instruction_ptr_type instruction){}
