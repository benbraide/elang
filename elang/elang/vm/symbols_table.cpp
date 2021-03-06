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

bool elang::vm::symbol_entry::is_storage() const{
	return false;
}

void elang::vm::symbol_entry::define(){
	ELANG_REMOVE(attributes_, attribute_type::undefined_);
}

std::string elang::vm::named_symbol_entry::mangle() const{
	std::string mangled_value;
	if (parent_ == nullptr)
		mangled_value =(std::to_string(name_.size()) + name_);
	else//Add parent's mangling
		mangled_value = (parent_->mangle() + std::to_string(name_.size()) + name_);

	return mangled_value;
}

bool elang::vm::named_symbol_entry::is(const std::string &name) const{
	return (name == name_);
}

elang::vm::symbol_entry::id_type elang::vm::variable_symbol_entry::id() const{
	return id_type::variable;
}

elang::vm::symbol_entry::size_type elang::vm::variable_symbol_entry::size() const{
	return type()->size();
}

elang::vm::symbol_entry::type_info_ptr_type elang::vm::variable_symbol_entry::type() const{
	return nullptr;
}

void elang::vm::variable_symbol_entry::add_reference(){}

elang::vm::symbol_entry::size_type elang::vm::variable_symbol_entry::reference_count() const{
	return 0u;
}

void elang::vm::variable_symbol_entry::set_stack_offset(size_type value){}

elang::vm::symbol_entry::size_type elang::vm::variable_symbol_entry::stack_offset() const{
	return static_cast<size_type>(-1);
}

elang::vm::symbol_entry::type_info_ptr_type elang::vm::basic_variable_symbol_entry::type() const{
	return type_;
}

void elang::vm::basic_variable_symbol_entry::add_reference(){
	++ref_count_;
}

elang::vm::symbol_entry::size_type elang::vm::basic_variable_symbol_entry::reference_count() const{
	return ref_count_;
}

void elang::vm::basic_variable_symbol_entry::set_stack_offset(size_type value){
	stack_offset_ = value;
}

elang::vm::symbol_entry::size_type elang::vm::basic_variable_symbol_entry::stack_offset() const{
	return stack_offset_;
}

elang::vm::symbol_entry::type_info_ptr_type elang::vm::reference_variable_symbol_entry::type() const{
	return type_;
}

void elang::vm::reference_variable_symbol_entry::add_reference(){
	target_->add_reference();
}

elang::vm::symbol_entry::size_type elang::vm::reference_variable_symbol_entry::reference_count() const{
	return target_->reference_count();
}

void elang::vm::reference_variable_symbol_entry::set_stack_offset(size_type value){
	target_->set_stack_offset(value);
}

elang::vm::symbol_entry::size_type elang::vm::reference_variable_symbol_entry::stack_offset() const{
	return target_->stack_offset();
}

bool elang::vm::storage_symbol_entry::is_storage() const{
	return true;
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
		else{//New entry
			map_[key] = value;
			order_list_.push_back(value);
		}
	}
	else//Add function
		add_(key, *function_entry);
}

elang::vm::variable_symbol_entry *elang::vm::storage_symbol_entry::add_variable(const std::string &name, type_info_ptr_type type, attribute_type attributes){
	if (map_.find(name) != map_.end())
		throw compiler_error::redefinition;

	auto entry = std::make_shared<basic_variable_symbol_entry>(type, name, this, attributes);
	if (!ELANG_IS(entry->attributes(), attribute_type::static_))//Update stack offset
		entry->set_stack_offset(get_stack_offset(entry->type()->size()));

	map_[name] = entry;
	order_list_.push_back(entry);

	return entry.get();
}

elang::vm::variable_symbol_entry *elang::vm::storage_symbol_entry::add_reference_variable(const std::string &name, variable_symbol_entry &target, attribute_type attributes){
	if (map_.find(name) != map_.end())
		throw compiler_error::redefinition;

	auto entry = std::make_shared<reference_variable_symbol_entry>(target, name, this, attributes);
	map_[name] = entry;
	order_list_.push_back(entry);

	return entry.get();
}

elang::vm::symbol_entry *elang::vm::storage_symbol_entry::find(const std::string &key) const{
	auto entry = map_.find(key);
	if ((entry == map_.end()))//Try storage if applicable
		return (::isdigit(key[0]) ? nullptr : find_storage(key));
	return entry->second.get();
}

elang::vm::symbol_entry *elang::vm::storage_symbol_entry::find_storage(const std::string &key) const{
	return (find(std::to_string(key.size()) + key));
}

elang::vm::symbol_entry *elang::vm::storage_symbol_entry::find_storage_or_any(const std::string &key) const{
	auto entry = map_.find(std::to_string(key.size()) + key);
	if ((entry != map_.end()))//Storage found
		return entry->second.get();
	return (((entry = map_.find(key)) == map_.end()) ? nullptr : entry->second.get());
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
		order_list_.push_back(function_list_entry);

		return function_list_entry.get();
	}

	if (entry->second->id() != id_type::function)
		throw compiler_error::redefinition;

	return dynamic_cast<function_list_symbol_entry *>(entry->second.get());
}

elang::vm::symbol_entry::id_type elang::vm::function_symbol_entry::id() const{
	return id_type::function;
}

elang::vm::symbol_entry::size_type elang::vm::function_symbol_entry::size() const{
	return size_;
}

elang::vm::symbol_entry::type_info_ptr_type elang::vm::function_symbol_entry::type() const{
	return ((type_ == nullptr) ? make_type_() : type_);
}

std::string elang::vm::function_symbol_entry::mangle() const{
	return (storage_symbol_entry::mangle() + dynamic_cast<function_type_info *>(type_.get())->mangle_parameters());
}

bool elang::vm::function_symbol_entry::is_storage() const{
	return false;
}

elang::vm::symbol_entry::size_type elang::vm::function_symbol_entry::get_stack_offset(size_type new_size){
	auto value = size_;
	size_ += new_size;
	return value;
}

elang::vm::variable_symbol_entry *elang::vm::function_symbol_entry::add_parameter(const std::string &name, type_info_ptr_type type){
	auto entry = add_variable(name, type, attribute_type::nil);
	if (entry != nullptr)//Add to parameter list
		parameter_order_list_.push_back(entry);
	return entry;
}

int elang::vm::function_symbol_entry::score(const type_info_ptr_list_type &parameter_types) const{
	auto arg = parameter_types.begin();
	auto &parameters = dynamic_cast<function_type_info *>(type().get())->parameters();
	auto iter = parameters.begin();

	int min_score = ELANG_TYPE_INFO_MIN_SCORE, score;
	for (; arg != parameter_types.end() && iter != parameters.end(); ++arg){
		if ((score = (*iter)->score(**arg)) == ELANG_TYPE_INFO_MIN_SCORE)
			return ELANG_TYPE_INFO_MIN_SCORE;//Mismatch

		if (score < min_score)//Update min
			min_score = score;

		if (!(*iter)->is_variadic())
			++iter;
	}

	if (arg != parameter_types.end())//All arguments not matched
		return ELANG_TYPE_INFO_MIN_SCORE;

	return ((iter == parameters.end() || (*iter)->is_optional()) ? min_score : ELANG_TYPE_INFO_MIN_SCORE);
}

elang::vm::symbol_entry::type_info_ptr_type elang::vm::function_symbol_entry::make_type_() const{
	type_info_ptr_list_type type_list;
	if (!parameter_order_list_.empty()){
		type_list.reserve(parameter_order_list_.size());
		for (auto param : parameter_order_list_){
			if (param->has_initialization())
				type_list.push_back(std::make_shared<optional_type_info>(param->type()));
			else//Parameter is not optional
				type_list.push_back(param->type());
		}
	}
	
	return (type_ = std::make_shared<function_type_info>(return_type_, std::move(type_list), type_info::attribute_type::nil));
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
		for (auto &entry : function_list->map_)
			add_(*dynamic_cast<function_symbol_entry *>(entry.second.get()));
	}
	else//Add single
		add_(*dynamic_cast<function_symbol_entry *>(value.get()));
}

elang::vm::symbol_entry *elang::vm::function_list_symbol_entry::resolve(const type_info_ptr_list_type &parameter_types, int *score) const{
	symbol_entry *match = nullptr;
	int match_count = 0, max_score = ELANG_TYPE_INFO_MIN_SCORE, this_score;

	for (auto &entry : map_){
		if ((this_score = dynamic_cast<function_symbol_entry *>(entry.second.get())->score(parameter_types)) > max_score){
			match = entry.second.get();
			match_count = 1;
			max_score = this_score;
		}
		else if (this_score == max_score)
			++match_count;
	}

	if (match_count > 1)
		throw 0;//#TODO: Throw ambiguous error

	if (score != nullptr)//Update ref
		*score = max_score;

	return match;
}

void elang::vm::function_list_symbol_entry::add_(function_symbol_entry &value){
	auto key = value.mangle();
	auto entry = find_(key);

	if (entry != nullptr && entry->parent() == parent()){//Entry already exists
		auto function_type = dynamic_cast<function_type_info *>(value.type().get());
		if (!ELANG_IS(entry->attributes(), attribute_type::undefined_) || ELANG_IS(value.attributes(), attribute_type::undefined_) || entry->mangle() != value.mangle()){
			if (dynamic_cast<function_type_info *>(entry->type().get())->return_type()->is_same(*function_type->return_type()))
				throw compiler_error::redefinition;
			throw compiler_error::unique_return_type;
		}
	}

	map_[key] = value.reflect();
}

elang::vm::function_symbol_entry *elang::vm::function_list_symbol_entry::find_(const std::string &value) const{
	auto entry = map_.find(value);
	return ((entry == map_.end()) ? nullptr : dynamic_cast<function_symbol_entry *>(entry->second.get()));
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

	machine::compiler.info().current_context.search_offset += size_;
	for (auto &base : base_map_){
		if ((value = base.second.value->find(key)) != nullptr)
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
		if ((value = base.second.value->find_base_or_this(key)) != nullptr)
			return value;
	}

	return nullptr;
}

void elang::vm::class_type_symbol_entry::add_base(const std::string &key, class_type_symbol_entry *value, attribute_type attributes){
	if (!is_direct_base(*value)){
		base_map_[key] = base_info_type{ value, attributes };
		base_order_list_.push_back(key);
	}
	else//Error
		throw compiler_error::multiple_base;
}

bool elang::vm::class_type_symbol_entry::is_direct_base(symbol_entry &value) const{
	for (auto &base : base_map_){
		if (base.second.value == &value)
			return true;
	}

	return false;
}

bool elang::vm::class_type_symbol_entry::is_base(symbol_entry &value) const{
	for (auto &base : base_map_){
		if (base.second.value == &value || base.second.value->is_base(value))
			return true;
	}

	return false;
}

std::string elang::vm::unnamed_class_type_symbol_entry::mangle() const{
	return name_;
}

elang::vm::symbol_entry::id_type elang::vm::namespace_symbol_entry::id() const{
	return id_type::namespace_;
}

std::string elang::vm::global_namespace_symbol_entry::mangle() const{
	return name_;
}
