#include "instruction_label.h"

elang::easm::instruction_label::instruction_label(instruction_label *parent, const std::string &name)
	: parent_(parent), name_(name){
	if (parent_ == nullptr)
		qualified_name_ = ("." + name_);
	else//Join names
		qualified_name_ = (parent_->qualified_name_ + "." + name_);
}

void elang::easm::instruction_label::create() const{

}

elang::easm::instruction_label *elang::easm::instruction_label::parent() const{
	return parent_;
}

int elang::easm::instruction_label::nested_level() const{
	return ((parent_ == nullptr) ? 0 : (parent_->nested_level() + 1));
}

const std::string &elang::easm::instruction_label::qualified_name() const{
	return qualified_name_;
}

elang::easm::instruction_label *elang::easm::instruction_label::find(const std::string &name, bool is_qualified) const{
	if (is_qualified){
		if (name == qualified_name_)
			return const_cast<instruction_label *>(this);

		return ((parent_ == nullptr) ? nullptr : parent_->find(name, is_qualified));//#TODO: Search all labels
	}

	if (name == name_)
		return const_cast<instruction_label *>(this);

	return ((parent_ == nullptr) ? nullptr : parent_->find(name, is_qualified));//#TODO: Search all labels
}

void elang::easm::instruction_label::print(writer_type &writer, writer_type &wide_writer) const{
	auto level_count = nested_level();
	if (level_count > 0)//Write prefix
		writer << std::string(level_count, '.');

	writer << name_ << writer_type::manip_type::flush;
}
