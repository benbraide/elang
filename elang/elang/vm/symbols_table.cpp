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

elang::vm::primitive_type_symbol_entry::primitive_type_symbol_entry(symbol_entry *parent, primitive_type_id_type primitive_type_id)
	: symbol_entry(parent, attribute_type::nil), primitive_type_id_(primitive_type_id){}

elang::vm::symbol_entry::id_type elang::vm::primitive_type_symbol_entry::id() const{
	return id_type::type;
}

elang::vm::symbol_entry::size_type elang::vm::primitive_type_symbol_entry::size() const{
	switch (primitive_type_id_){
	case elang::common::primitive_type_id::int8_:
	case elang::common::primitive_type_id::uint8_:
		return 1u;
	case elang::common::primitive_type_id::int16_:
	case elang::common::primitive_type_id::uint16_:
		return 2u;
	case elang::common::primitive_type_id::int32_:
	case elang::common::primitive_type_id::uint32_:
		return 4u;
	default:
		break;
	}

	return 8u;
}

std::string elang::vm::primitive_type_symbol_entry::mangle() const{
	switch (primitive_type_id_){
	case primitive_type_id_type::auto_:
		return "a";
	case primitive_type_id_type::void_:
		return "v";
	case primitive_type_id_type::nullptr_:
		return "n";
	case primitive_type_id_type::bool_:
		return "b";
	case primitive_type_id_type::char_:
		return "c";
	case primitive_type_id_type::wchar_:
		return "w";
	case primitive_type_id_type::int8_:
		return "g";
	case primitive_type_id_type::uint8_:
		return "d";
	case primitive_type_id_type::int16_:
		return "s";
	case primitive_type_id_type::uint16_:
		return "e";
	case primitive_type_id_type::int32_:
		return "i";
	case primitive_type_id_type::uint32_:
		return "u";
	case primitive_type_id_type::int64_:
		return "l";
	case primitive_type_id_type::uint64_:
		return "q";
	case primitive_type_id_type::float_:
		return "f";
	default:
		break;
	}

	return "";
}

bool elang::vm::primitive_type_symbol_entry::is(const std::string &name) const{
	return false;
}

elang::vm::primitive_type_symbol_entry::primitive_type_id_type elang::vm::primitive_type_symbol_entry::primitive_type_id() const{
	return primitive_type_id_;
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

elang::vm::symbol_entry::id_type elang::vm::variable_symbol_entry::id() const{
	return id_type::variable;
}

elang::vm::symbol_entry::type_info_ptr_type elang::vm::variable_symbol_entry::type() const{
	return type_;
}

elang::vm::symbol_entry::id_type elang::vm::function_symbol_entry::id() const{
	return id_type::function;
}

std::string elang::vm::function_symbol_entry::mangle() const{
	return (variable_symbol_entry::mangle() + dynamic_cast<function_type_info *>(type_.get())->mangle_parameters());
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
	bool mismatch;
	for (auto entry : list_){
		auto &parameters = dynamic_cast<function_type_info *>(entry->type().get())->parameters();
		if (parameters.size() != parameter_types.size())
			continue;

		mismatch = false;
		for (auto l = parameters.begin(), r = parameter_types.begin(); l == parameters.end(); ++l, ++r){
			if (!(*l)->is_compatible(**r)){//Different types
				mismatch = true;
				break;
			}
		}

		if (!mismatch)
			return entry.get();
	}

	return nullptr;
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

void elang::vm::type_symbol_entry::add(const std::string &key, ptr_type value){
	storage_symbol_entry::add(key, value);
	if (value->id() == id_type::variable)
		update_size_(*value);
}

void elang::vm::enum_type_symbol_entry::update_size_(symbol_entry &value){}

void elang::vm::union_type_symbol_entry::update_size_(symbol_entry &value){
	if (size_ < value.size())//use max
		size_ = value.size();
}

void elang::vm::struct_type_symbol_entry::update_size_(symbol_entry &value){
	size_ += value.size();
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

void elang::vm::class_type_symbol_entry::update_size_(symbol_entry &value){
	if (!ELANG_IS(value.attributes(), attribute_type::static_))
		size_ += value.size();
}

elang::vm::symbol_entry::id_type elang::vm::namespace_symbol_entry::id() const{
	return id_type::namespace_;
}

std::string elang::vm::namespace_symbol_entry::mangle() const{
	if (parent_ == nullptr)
		return (std::to_string(name_.size()) + name_);
	return (parent_->mangle() + std::to_string(name_.size()) + name_);
}
