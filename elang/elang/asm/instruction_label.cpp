#include "instruction_label.h"

elang::easm::instruction_label::instruction_label(instruction_label *parent, const std::string &name)
	: parent_(parent), name_(name){}

elang::easm::instruction_label *elang::easm::instruction_label::add(const std::string &label){
	return nullptr;
}

void elang::easm::instruction_label::create() const{

}

elang::easm::instruction_label *elang::easm::instruction_label::parent() const{
	return parent_;
}

int elang::easm::instruction_label::nested_level() const{
	return ((parent_ == nullptr) ? 0 : (parent_->nested_level() + 1));
}

elang::easm::instruction_label *elang::easm::instruction_label::find(const std::string &first, const std::vector<std::string> &rest) const{
	if (first == name_){
		if (rest.empty())
			return const_cast<instruction_label *>(this);

		instruction_label *found = nullptr;
		for (auto entry : list_){//Search tree
			if ((found = entry->find_(rest.begin(), rest.end())) != nullptr)
				return found;
		}

		return nullptr;
	}

	return ((parent_ == nullptr) ? nullptr : parent_->find(first, rest));
}

void elang::easm::instruction_label::print(writer_type &writer, writer_type &wide_writer) const{
	auto level_count = nested_level();
	if (level_count > 0)//Write prefix
		writer << std::string(level_count, '.');

	writer << name_ << writer_type::manip_type::flush;
}

elang::easm::instruction_label *elang::easm::instruction_label::find_(string_list_iterator_type iter, string_list_iterator_type end) const{
	if (*iter != name_)//Name does not match this
		return nullptr;

	if (++iter == end)//Match completed
		return const_cast<instruction_label *>(this);

	instruction_label *found = nullptr;
	for (auto entry : list_){//Search tree
		if ((found = entry->find_(iter, end)) != nullptr)
			return found;
	}

	return nullptr;
}
