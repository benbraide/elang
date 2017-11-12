#include "../asm/instruction_operand/constant_value_instruction_operand.h"
#include "../asm/instruction_operand/expression_instruction_operand.h"
#include "../asm/instruction_operand/register_instruction_operand.h"
#include "../asm/instruction_operand/label_instruction_operand.h"
#include "../asm/instruction_operand/memory_instruction_operand.h"

#include "machine.h"
#include "symbols_table.h"

elang::vm::symbol_entry::symbol_entry(symbol_entry *parent, attribute_type attributes)
	: parent_(parent), attributes_(attributes){}

elang::vm::symbol_entry::ptr_type elang::vm::symbol_entry::reflect(){
	return shared_from_this();
}

elang::vm::symbol_entry::attribute_type elang::vm::symbol_entry::attributes() const{
	return attributes_;
}

elang::vm::symbol_entry *elang::vm::symbol_entry::parent() const{
	return parent_;
}

elang::vm::symbol_entry::size_type elang::vm::symbol_entry::size() const{
	return 0u;
}

elang::vm::symbol_entry::type_info_ptr_type elang::vm::symbol_entry::type() const{
	return nullptr;
}

void elang::vm::symbol_entry::define(){
	ELANG_REMOVE(attributes_, attribute_type::undefined_);
}

std::string elang::vm::named_symbol_entry::mangle() const{
	auto &symbol_mangling = machine::compiler.info().symbol_mangling;
	if (symbol_mangling == nullptr)
		symbol_mangling = this;

	std::string mangled_value;
	if (parent_ == nullptr)
		mangled_value =(std::to_string(name_.size()) + name_);
	else
		mangled_value = (parent_->mangle() + std::to_string(name_.size()) + name_);

	if (symbol_mangling == nullptr || symbol_mangling == this){
		symbol_mangling = nullptr;
		return ("N" + mangled_value + "E");
	}

	return mangled_value;
}

bool elang::vm::named_symbol_entry::is(const std::string &name) const{
	return (name == name_);
}

void elang::vm::storage_symbol_entry::add(const std::string &key, ptr_type value){
	auto function_entry = dynamic_cast<function_symbol_entry *>(value.get());
	if (function_entry == nullptr){
		auto entry = map_.find(key);
		if (entry != map_.end()){
			if (!ELANG_IS(entry->second->attributes(), attribute_type::undefined_) || ELANG_IS(value->attributes(), attribute_type::undefined_))
				throw compiler_error::redefinition;//Existing is already defined | New is also not defined

			if (entry->second->mangle() != value->mangle())
				throw compiler_error::redefinition;//Different types

			entry->second->define();
		}
		else//New entry
			map_[key] = value;
	}
	else//Add function
		add_(key, *function_entry);
}

elang::vm::symbol_entry *elang::vm::storage_symbol_entry::find(const std::string &key) const{
	auto entry = map_.find(key);
	return ((entry == map_.end()) ? nullptr : entry->second.get());
}

elang::vm::symbol_entry::size_type elang::vm::storage_symbol_entry::get_stack_offset(size_type new_size){
	return static_cast<size_type>(-1);
}

void elang::vm::storage_symbol_entry::add_(const std::string &key, function_symbol_entry &value){
	get_function_list_(key)->add(value.reflect());
}

elang::vm::function_list_symbol_entry *elang::vm::storage_symbol_entry::get_function_list_(const std::string &key){
	auto entry = map_.find(key);
	if (entry == map_.end()){//Add new
		auto function_list_entry = std::make_shared<function_list_symbol_entry>(key, this, attribute_type::nil);
		map_[key] = function_list_entry;
		return function_list_entry.get();
	}

	if (entry->second->id() != id_type::function)
		throw compiler_error::redefinition;

	return dynamic_cast<function_list_symbol_entry *>(entry->second.get());
}

elang::vm::symbol_entry::id_type elang::vm::variable_symbol_entry::id() const{
	return id_type::variable;
}

elang::vm::symbol_entry::type_info_ptr_type elang::vm::variable_symbol_entry::type() const{
	return type_;
}

elang::vm::variable_symbol_entry::instruction_operand_ptr_type elang::vm::variable_symbol_entry::reference(){
	++ref_count_;
	if (ref_ != nullptr)
		return ref_;

	instruction_operand_ptr_type offset;
	if (stack_offset_ != static_cast<size_type>(-1)){
		auto constant = std::make_shared<elang::easm::instruction::constant_value_operand<__int64>>(stack_offset_);
		auto reg = std::make_shared<elang::easm::instruction::register_operand>(*machine::cached_registers.stack_pointer);
		offset = std::make_shared<elang::easm::instruction::expression_operand>(elang::easm::instruction_operator_id::add, reg, constant);
	}
	else//Static or global variable
		offset = std::make_shared<elang::easm::instruction::label_operand>(mangle(), std::vector<std::string>{});

	return (ref_ = std::make_shared<elang::easm::instruction::memory_operand>(elang::vm::machine_value_type_id::qword, offset));
}

elang::vm::symbol_entry::size_type elang::vm::variable_symbol_entry::reference_count() const{
	return ref_count_;
}

elang::vm::symbol_entry::id_type elang::vm::function_symbol_entry::id() const{
	return id_type::function;
}

elang::vm::symbol_entry::size_type elang::vm::function_symbol_entry::size() const{
	return size_;
}

elang::vm::symbol_entry::type_info_ptr_type elang::vm::function_symbol_entry::type() const{
	return type_;
}

std::string elang::vm::function_symbol_entry::mangle() const{
	return (storage_symbol_entry::mangle() + dynamic_cast<function_type_info *>(type_.get())->mangle_parameters());
}

elang::vm::symbol_entry::size_type elang::vm::function_symbol_entry::get_stack_offset(size_type new_size){
	auto value = size_;
	size_ += new_size;
	return value;
}

elang::vm::symbol_entry::id_type elang::vm::function_list_symbol_entry::id() const{
	return id_type::function;
}

std::string elang::vm::function_list_symbol_entry::mangle() const{
	throw compiler_error::ambiguous_function;
}

void elang::vm::function_list_symbol_entry::add(ptr_type value){
	auto function_list = dynamic_cast<function_list_symbol_entry *>(value.get());
	if (function_list != nullptr){//Add multiple
		for (auto entry : function_list->list_)
			add_(*dynamic_cast<function_symbol_entry *>(entry.get()));
	}
	else//Add single
		add_(*dynamic_cast<function_symbol_entry *>(value.get()));
}

elang::vm::symbol_entry *elang::vm::function_list_symbol_entry::find(const type_info_ptr_list_type &parameter_types) const{
	bool mismatch;
	for (auto entry : list_){
		auto &parameters = dynamic_cast<function_type_info *>(entry->type().get())->parameters();
		if (parameters.size() != parameter_types.size())
			continue;

		mismatch = false;
		for (auto l = parameters.begin(), r = parameter_types.begin(); l == parameters.end(); ++l, ++r){
			if (!(*l)->is_same(**r)){//Different types
				mismatch = true;
				break;
			}
		}

		if (!mismatch)
			return entry.get();
	}

	return nullptr;
}

elang::vm::symbol_entry *elang::vm::function_list_symbol_entry::resolve(const type_info_ptr_list_type &parameter_types) const{
	bool mismatch, same;
	symbol_entry *match = nullptr, *compatible = nullptr;

	for (auto entry : list_){
		auto &parameters = dynamic_cast<function_type_info *>(entry->type().get())->parameters();
		if (parameters.size() != parameter_types.size())
			continue;

		mismatch = same = true;
		for (auto l = parameters.begin(), r = parameter_types.begin(); l == parameters.end(); ++l, ++r){
			if (match == nullptr && same && !(*l)->is_compatible(**r)){//Different types
				same = false;
				if (!(*l)->is_compatible(**r)){
					mismatch = true;
					break;
				}
			}
			else if (!(*l)->is_compatible(**r)){
				mismatch = true;
				break;
			}
		}

		if (!mismatch){//Match found
			if (same){
				match = entry.get();
				break;
			}
			else if (compatible == nullptr)//Compatible
				compatible = entry.get();
		}
	}

	return ((match == nullptr) ? compatible : match);
}

void elang::vm::function_list_symbol_entry::add_(function_symbol_entry &value){
	auto function_type = dynamic_cast<function_type_info *>(value.type().get());
	auto entry = dynamic_cast<function_symbol_entry *>(find(function_type->parameters()));
	if (entry != nullptr){//Entry already exists
		if (!ELANG_IS(entry->attributes(), attribute_type::undefined_) || ELANG_IS(value.attributes(), attribute_type::undefined_) || entry->mangle() != value.mangle()){
			if (dynamic_cast<function_type_info *>(entry->type().get())->return_type()->is_same(*function_type->return_type()))
				throw compiler_error::redefinition;
			throw compiler_error::unique_return_type;
		}
	}

	list_.push_back(value.reflect());
}

elang::vm::symbol_entry::id_type elang::vm::type_symbol_entry::id() const{
	return id_type::type;
}

elang::vm::symbol_entry::size_type elang::vm::type_symbol_entry::size() const{
	if (ELANG_IS(attributes(), attribute_type::undefined_))
		throw compiler_error::undefined_type;
	return size_;
}

elang::vm::symbol_entry::size_type elang::vm::union_type_symbol_entry::get_stack_offset(size_type new_size){
	return 0u;
}

elang::vm::symbol_entry::size_type elang::vm::struct_type_symbol_entry::get_stack_offset(size_type new_size){
	auto value = size_;
	size_ += new_size;
	return value;
}

elang::vm::symbol_entry::size_type elang::vm::class_type_symbol_entry::get_stack_offset(size_type new_size){
	auto value = size_;
	size_ += new_size;
	return value;
}

elang::vm::symbol_entry *elang::vm::class_type_symbol_entry::find(const std::string &key) const{
	auto value = type_symbol_entry::find(key);
	if (value != nullptr)
		return value;

	for (auto &base : base_map_){
		if ((value = dynamic_cast<class_type_symbol_entry *>(base.second.value.get())->find(key)) != nullptr)
			return value;
	}

	return nullptr;
}

elang::vm::symbol_entry *elang::vm::class_type_symbol_entry::find_inside_this(const std::string &key) const{
	return type_symbol_entry::find(key);
}

elang::vm::symbol_entry *elang::vm::class_type_symbol_entry::find_base_or_this(const std::string &key){
	if (is(key))
		return this;

	symbol_entry *value;
	for (auto &base : base_map_){
		if ((value = dynamic_cast<class_type_symbol_entry *>(base.second.value.get())->find_base_or_this(key)) != nullptr)
			return value;
	}

	return nullptr;
}

void elang::vm::class_type_symbol_entry::add_base(const std::string &key, ptr_type value, attribute_type attributes){
	if (!is_direct_base(*value)){
		base_map_[key] = base_info_type{ value, attributes };
		base_order_list_.push_back(key);
	}
	else//Error
		throw compiler_error::multiple_base;
}

bool elang::vm::class_type_symbol_entry::is_direct_base(symbol_entry &value) const{
	for (auto &base : base_map_){
		if (base.second.value.get() == &value)
			return true;
	}

	return false;
}

bool elang::vm::class_type_symbol_entry::is_base(symbol_entry &value) const{
	for (auto &base : base_map_){
		if (base.second.value.get() == &value || dynamic_cast<class_type_symbol_entry *>(base.second.value.get())->is_base(value))
			return true;
	}

	return false;
}

elang::vm::symbol_entry::id_type elang::vm::namespace_symbol_entry::id() const{
	return id_type::namespace_;
}

std::string elang::vm::namespace_symbol_entry::mangle() const{
	if (parent_ == nullptr)
		return (std::to_string(name_.size()) + name_);
	return (parent_->mangle() + std::to_string(name_.size()) + name_);
}
