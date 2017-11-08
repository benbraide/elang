#include "type_info.h"

elang::vm::machine_value_type_id elang::vm::type_info::id() const{
	return machine_value_type_id::unknown;
}

elang::vm::type_info * elang::vm::type_info::underlying_type() const{
	throw error_type::not_applicable;
}

elang::vm::type_info::entry_type * elang::vm::type_info::find(const std::string &key) const{
	throw error_type::not_applicable;
}

bool elang::vm::type_info::is_compatible(const type_info &type) const{
	return is_same(type);
}

bool elang::vm::type_info::is_primitive() const{
	return false;
}

bool elang::vm::type_info::is_integral() const{
	return false;
}

bool elang::vm::type_info::is_pointer() const{
	return false;
}

bool elang::vm::type_info::is_array() const{
	return false;
}

bool elang::vm::type_info::is_function() const{
	return false;
}

bool elang::vm::type_info::is_void() const{
	return false;
}

bool elang::vm::type_info::is_bool() const{
	return false;
}

bool elang::vm::type_info::is_storage() const{
	return false;
}

bool elang::vm::type_info::is_ref() const{
	return false;
}

bool elang::vm::type_info::is_vref() const{
	return false;
}

elang::vm::primitive_type_info::primitive_type_info(index_type index)
	: index_(index){}

elang::vm::machine_value_type_id elang::vm::primitive_type_info::id() const{
	switch (index_){
	case index_type::int8_:
	case index_type::uint8_:
		return machine_value_type_id::byte;
	case index_type::int16_:
	case index_type::uint16_:
		return machine_value_type_id::word;
	case index_type::int32_:
	case index_type::uint32_:
		return machine_value_type_id::dword;
	case index_type::int64_:
	case index_type::uint64_:
		return machine_value_type_id::qword;
	case index_type::float_:
		return machine_value_type_id::float_;
	default:
		break;
	}

	throw error_type::unreachable;
}

elang::vm::type_info::size_type elang::vm::primitive_type_info::size() const{
	switch (id()){
	case machine_value_type_id::byte:
		return 1u;
	case machine_value_type_id::word:
		return 2u;
	case machine_value_type_id::dword:
		return 4u;
	case machine_value_type_id::qword:
	case machine_value_type_id::float_:
		return 8u;
	default:
		break;
	}

	throw error_type::unreachable;
}

bool elang::vm::primitive_type_info::is_same(const type_info &type) const{
	auto primitive_type = dynamic_cast<const primitive_type_info *>(&type);
	return (primitive_type != nullptr && primitive_type->index_ == index_);
}

bool elang::vm::primitive_type_info::is_compatible(const type_info &type) const{
	return (type.id() != machine_value_type_id::unknown);
}

bool elang::vm::primitive_type_info::is_primitive() const{
	return true;
}

bool elang::vm::primitive_type_info::is_integral() const{
	return (index_ != index_type::float_);
}

elang::vm::pointer_type_info::pointer_type_info(ptr_type value)
	: value_(value){}

elang::vm::type_info::size_type elang::vm::pointer_type_info::size() const{
	return 8u;
}

elang::vm::type_info *elang::vm::pointer_type_info::underlying_type() const{
	return value_.get();
}

bool elang::vm::pointer_type_info::is_same(const type_info &type) const{
	return (type.is_pointer() && type.underlying_type()->is_same(*value_));
}

bool elang::vm::pointer_type_info::is_compatible(const type_info &type) const{
	return ((type.is_pointer() || type.is_array()) && (value_->is_void() || value_->is_same(*type.underlying_type())));
}

bool elang::vm::pointer_type_info::is_pointer() const{
	return true;
}

elang::vm::array_type_info::array_type_info(ptr_type value, size_type count)
	: value_(value), count_(count){}

elang::vm::type_info::size_type elang::vm::array_type_info::size() const{
	return (value_->size() * count_);
}

elang::vm::type_info *elang::vm::array_type_info::underlying_type() const{
	return value_.get();
}

bool elang::vm::array_type_info::is_same(const type_info &type) const{
	return (type.is_array() && type.underlying_type()->is_same(*value_));
}

bool elang::vm::array_type_info::is_array() const{
	return true;
}

elang::vm::function_type_info::function_type_info(ptr_type return_type, const ptr_list_type &parameters)
	: return_type_(return_type), parameters_(parameters){}

elang::vm::function_type_info::function_type_info(ptr_type return_type, ptr_list_type &&parameters)
	: return_type_(return_type), parameters_(std::move(parameters)){}

elang::vm::type_info::size_type elang::vm::function_type_info::size() const{
	return 8u;
}

bool elang::vm::function_type_info::is_same(const type_info &type) const{
	auto function_type = dynamic_cast<const function_type_info *>(&type);
	return (function_type != nullptr && is_same_return_type(*function_type) && is_same_parameters(*function_type));
}

bool elang::vm::function_type_info::is_function() const{
	return true;
}

bool elang::vm::function_type_info::is_same_return_type(const function_type_info &type) const{
	return type.return_type_->is_same(*return_type_);
}

bool elang::vm::function_type_info::is_same_parameters(const function_type_info &type) const{
	if (type.parameters_.size() != parameters_.size())
		return false;//Parameter count mismatch

	for (auto lparam = parameters_.begin(), rparam = type.parameters_.begin(); lparam != parameters_.end(); ++lparam, ++rparam){
		if (!(*lparam)->is_same(**rparam))//Parameter mismatch
			return false;
	}

	return true;
}

elang::vm::type_info::size_type elang::vm::function_list_type_info::size() const{
	return 8u;
}

bool elang::vm::function_list_type_info::is_same(const type_info &type) const{
	for (auto &entry : list_){
		if (entry.value->is_same(type))
			return true;
	}

	return false;
}

bool elang::vm::function_list_type_info::is_function() const{
	return true;
}

void elang::vm::function_list_type_info::add(ptr_type value, attribute_type attributes){
	auto function_list = dynamic_cast<function_list_type_info *>(value.get());
	if (function_list != nullptr){//Add list
		for (auto &entry : function_list->list_)
			add_(entry.value, entry.attributes);
	}
	else//Add single
		add_(value, attributes);
}

void elang::vm::function_list_type_info::add_(ptr_type value, attribute_type attributes){
	auto function_type = dynamic_cast<function_type_info *>(value.get());
	if (function_type == nullptr)
		throw error_type::redefinition;

	auto same_parameters = get_same_parameters_(*function_type);
	if (same_parameters != nullptr){//Error
		if (same_parameters->is_same_return_type(*function_type))
			throw error_type::redefinition;
		else//Same parameter types, different return types
			throw error_type::unique_return_type_only;
	}
	else//Add entry
		list_.push_back(entry_type{ value, attributes });
}

elang::vm::function_type_info *elang::vm::function_list_type_info::get_same_parameters_(const function_type_info &function_type) const{
	function_type_info *found = nullptr;
	for (auto &entry : list_){
		if ((found = dynamic_cast<function_type_info *>(entry.value.get())) != nullptr && found->is_same_parameters(function_type))
			break;//Entry found
	}

	return found;
}

elang::vm::type_info::size_type elang::vm::bool_type_info::size() const{
	return 1u;
}

bool elang::vm::bool_type_info::is_same(const type_info &type) const{
	return type.is_bool();
}

bool elang::vm::bool_type_info::is_bool() const{
	return true;
}

elang::vm::type_info::size_type elang::vm::void_type_info::size() const{
	throw error_type::not_applicable;
}

bool elang::vm::void_type_info::is_same(const type_info &type) const{
	return type.is_void();
}

bool elang::vm::void_type_info::is_void() const{
	return true;
}

elang::vm::storage_type_info::storage_type_info()
	: size_(0){}

elang::vm::storage_type_info::storage_type_info(const map_type &map)
	: map_(map), size_(0){
	compute_size_();
}

elang::vm::storage_type_info::storage_type_info(map_type &&map)
	: map_(std::move(map)), size_(0){
	compute_size_();
}

elang::vm::type_info::size_type elang::vm::storage_type_info::size() const{
	return size_;
}

elang::vm::type_info::entry_type *elang::vm::storage_type_info::find(const std::string &key) const{
	auto entry = map_.find(key);
	return ((entry == map_.end()) ? nullptr : const_cast<entry_type *>(&entry->second));
}

bool elang::vm::storage_type_info::is_same(const type_info &type) const{
	return (&type == this);
}

bool elang::vm::storage_type_info::is_storage() const{
	return true;
}

void elang::vm::storage_type_info::add(const std::string &key, ptr_type value, attribute_type attributes){
	if (value->is_function()){//Add to function list
		auto function_type = get_function_list_(key);
		if (function_type == nullptr)//Existing is not a function list
			throw error_type::redefinition;
		function_type->add(value, attributes);
	}
	else if (map_.find(key) == map_.end()){//Add unique entry
		map_[key] = entry_type{ value, attributes };
		if (!ELANG_IS(attributes, attribute_type::static_))
			update_size_(value->size());//Update size
	}
	else//Redefinition
		throw error_type::redefinition;
}

void elang::vm::storage_type_info::compute_size_(){
	for (auto &entry : map_){
		if (!ELANG_IS(entry.second.attributes, attribute_type::static_))
			size_ += entry.second.value->size();
	}
}

elang::vm::function_list_type_info *elang::vm::storage_type_info::get_function_list_(const std::string &key){
	auto entry = map_.find(key);
	if (entry != map_.end())//Return existing
		return dynamic_cast<function_list_type_info *>(entry->second.value.get());

	auto function_type = std::make_shared<function_list_type_info>();
	map_[key] = entry_type{ function_type };//Add entry

	return function_type.get();
}

void elang::vm::storage_type_info::update_size_(size_type size){
	size_ += size;
}

void elang::vm::union_type_info::compute_size_(){
	for (auto &entry : map_){//Find max size
		if (size_ < entry.second.value->size())
			size_ = entry.second.value->size();
	}
}

void elang::vm::union_type_info::update_size_(size_type size){
	if (size_ < size)//Use max
		size_ = size;
}

elang::vm::proxy_type_info::proxy_type_info(ptr_type value)
	: value_(value){}

elang::vm::machine_value_type_id elang::vm::proxy_type_info::id() const{
	return value_->id();
}

elang::vm::type_info::size_type elang::vm::proxy_type_info::size() const{
	return value_->size();
}

elang::vm::type_info *elang::vm::proxy_type_info::underlying_type() const{
	return value_->underlying_type();
}

elang::vm::type_info::entry_type *elang::vm::proxy_type_info::find(const std::string &key) const{
	return value_->find(key);
}

bool elang::vm::proxy_type_info::is_same(const type_info &type) const{
	return value_->is_same(type);
}

bool elang::vm::proxy_type_info::is_compatible(const type_info &type) const{
	return value_->is_compatible(type);
}

bool elang::vm::proxy_type_info::is_primitive() const{
	return value_->is_primitive();
}

bool elang::vm::proxy_type_info::is_integral() const{
	return value_->is_integral();
}

bool elang::vm::proxy_type_info::is_pointer() const{
	return value_->is_pointer();
}

bool elang::vm::proxy_type_info::is_array() const{
	return value_->is_array();
}

bool elang::vm::proxy_type_info::is_function() const{
	return value_->is_function();
}

bool elang::vm::proxy_type_info::is_void() const{
	return value_->is_void();
}

bool elang::vm::proxy_type_info::is_bool() const{
	return value_->is_bool();
}

bool elang::vm::proxy_type_info::is_storage() const{
	return value_->is_storage();
}

bool elang::vm::proxy_type_info::is_ref() const{
	return value_->is_ref();
}

bool elang::vm::proxy_type_info::is_vref() const{
	return value_->is_vref();
}

bool elang::vm::ref_type_info::is_ref() const{
	return true;
}

bool elang::vm::vref_type_info::is_vref() const{
	return true;
}
