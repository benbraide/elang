#include "machine.h"
#include "type_info.h"

elang::vm::type_info::type_info(attribute_type attributes)
	: attributes_(attributes){}

elang::vm::type_info::ptr_type elang::vm::type_info::reflect(){
	return shared_from_this();
}

elang::vm::machine_value_type_id elang::vm::type_info::id() const{
	return machine_value_type_id::unknown;
}

elang::vm::type_info::primitive_id_type elang::vm::type_info::primitive_id() const{
	return primitive_id_type::auto_;
}

elang::vm::type_info::attribute_type elang::vm::type_info::attributes() const{
	return attributes_;
}

elang::vm::type_info *elang::vm::type_info::underlying_type() const{
	return nullptr;
}

std::string elang::vm::type_info::mangle_attributes() const{
	std::string mangled_attributes;
	if (is_const())
		mangled_attributes = "C";

	if (is_ref())
		mangled_attributes += "R";
	else if (is_vref())
		mangled_attributes += "V";

	return mangled_attributes;
}

bool elang::vm::type_info::is_same_object(const type_info &type) const{
	return false;
}

bool elang::vm::type_info::is_compatible(const type_info &type) const{
	return is_same(type);
}

bool elang::vm::type_info::is_null_pointer() const{
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

bool elang::vm::type_info::is_numeric() const{
	return false;
}

bool elang::vm::type_info::is_integral() const{
	return false;
}

bool elang::vm::type_info::is_const() const{
	return ELANG_IS(attributes_, attribute_type::const_);
}

bool elang::vm::type_info::is_ref() const{
	return ELANG_IS(attributes_, attribute_type::ref_);
}

bool elang::vm::type_info::is_vref() const{
	return ELANG_IS(attributes_, attribute_type::vref);
}

bool elang::vm::type_info::is_variadic() const{
	return false;
}

bool elang::vm::type_info::is_same_(const type_info &type) const{
	return (is_ref() == type.is_ref() && is_vref() == type.is_vref());
}

bool elang::vm::type_info::is_compatible_(const type_info &type) const{
	return (is_vref() == type.is_vref() && (!is_ref() || type.is_ref()));
}

elang::vm::primitive_type_info::primitive_type_info(primitive_id_type id, attribute_type attributes)
	: type_info(attributes), id_(id){}

elang::vm::type_info::ptr_type elang::vm::primitive_type_info::clone(attribute_type attributes) const{
	return std::make_shared<primitive_type_info>(id_, attributes);
}

elang::vm::machine_value_type_id elang::vm::primitive_type_info::id() const{
	switch (id_){
	case elang::common::primitive_type_id::int8_:
	case elang::common::primitive_type_id::uint8_:
		return machine_value_type_id::byte;
	case elang::common::primitive_type_id::int16_:
	case elang::common::primitive_type_id::uint16_:
		return machine_value_type_id::word;
	case elang::common::primitive_type_id::int32_:
	case elang::common::primitive_type_id::uint32_:
		return machine_value_type_id::dword;
	case elang::common::primitive_type_id::int64_:
	case elang::common::primitive_type_id::uint64_:
		return machine_value_type_id::qword;
	case elang::common::primitive_type_id::float_:
		return machine_value_type_id::float_;
	default:
		break;
	}

	return machine_value_type_id::unknown;
}

elang::vm::type_info::primitive_id_type elang::vm::primitive_type_info::primitive_id() const{
	return id_;
}

elang::vm::type_info::size_type elang::vm::primitive_type_info::size() const{
	switch (id_){
	case primitive_id_type::char_:
	case primitive_id_type::int8_:
	case primitive_id_type::uint8_:
		return 1u;
	case primitive_id_type::wchar_:
	case primitive_id_type::int16_:
	case primitive_id_type::uint16_:
		return 2u;
	case primitive_id_type::int32_:
	case primitive_id_type::uint32_:
		return 4u;
	case primitive_id_type::int64_:
	case primitive_id_type::uint64_:
	case primitive_id_type::float_:
	case primitive_id_type::nullptr_:
		return 8u;
	default:
		break;
	}

	return 0u;
}

std::string elang::vm::primitive_type_info::mangle() const{
	return (mangle_attributes() + mangle_());
}

bool elang::vm::primitive_type_info::is_same(const type_info &type) const{
	if (!is_same_(type))
		return false;

	auto basic_type = dynamic_cast<const primitive_type_info *>(&type);
	return (basic_type != nullptr && basic_type->id_ == id_);
}

bool elang::vm::primitive_type_info::is_compatible(const type_info &type) const{
	if (is_ref())
		return is_same(type);
	return (is_numeric() == type.is_numeric());
}

bool elang::vm::primitive_type_info::is_null_pointer() const{
	return (id_ == primitive_id_type::nullptr_);
}

bool elang::vm::primitive_type_info::is_void() const{
	return (id_ == primitive_id_type::void_);
}

bool elang::vm::primitive_type_info::is_bool() const{
	return (id_ == primitive_id_type::bool_);
}

bool elang::vm::primitive_type_info::is_numeric() const{
	return (id_ >= primitive_id_type::char_ && id_ <= primitive_id_type::float_);
}

bool elang::vm::primitive_type_info::is_integral() const{
	return (id_ >= primitive_id_type::char_ && id_ <= primitive_id_type::uint64_);
}

std::string elang::vm::primitive_type_info::mangle_() const{
	switch (id_){
	case primitive_id_type::auto_:
		return "a";
	case primitive_id_type::void_:
		return "v";
	case primitive_id_type::nullptr_:
		return "n";
	case primitive_id_type::bool_:
		return "b";
	case primitive_id_type::char_:
		return "c";
	case primitive_id_type::wchar_:
		return "w";
	case primitive_id_type::int8_:
		return "g";
	case primitive_id_type::uint8_:
		return "d";
	case primitive_id_type::int16_:
		return "s";
	case primitive_id_type::uint16_:
		return "e";
	case primitive_id_type::int32_:
		return "i";
	case primitive_id_type::uint32_:
		return "u";
	case primitive_id_type::int64_:
		return "l";
	case primitive_id_type::uint64_:
		return "q";
	case primitive_id_type::float_:
		return "f";
	default:
		break;
	}

	throw compiler_error::unreachable;
}

elang::vm::user_type_info::user_type_info(symbol_ptr_type value, attribute_type attributes)
	: type_info(attributes), value_(value){}

elang::vm::type_info::ptr_type elang::vm::user_type_info::clone(attribute_type attributes) const{
	return std::make_shared<user_type_info>(value_, attributes);
}

elang::vm::type_info::size_type elang::vm::user_type_info::size() const{
	return value_->size();
}

std::string elang::vm::user_type_info::mangle() const{
	return (mangle_attributes() + value_->mangle());
}

bool elang::vm::user_type_info::is_same(const type_info &type) const{
	if (!is_same_(type))
		return false;

	auto basic_type = dynamic_cast<const user_type_info *>(&type);
	return (basic_type != nullptr && basic_type->value_ == value_);
}

bool elang::vm::user_type_info::is_compatible(const type_info &type) const{
	if (is_ref())
		return is_same(type);
	return type_info::is_compatible(type);//#TODO: Implement
}

elang::vm::pointer_type_info::pointer_type_info(ptr_type value, attribute_type attributes)
	: type_info(attributes), value_(value){}

elang::vm::type_info::ptr_type elang::vm::pointer_type_info::clone(attribute_type attributes) const{
	return std::make_shared<pointer_type_info>(value_, attributes);
}

elang::vm::type_info::size_type elang::vm::pointer_type_info::size() const{
	return 8u;
}

elang::vm::type_info *elang::vm::pointer_type_info::underlying_type() const{
	return value_.get();
}

std::string elang::vm::pointer_type_info::mangle() const{
	return ("P" + mangle_attributes() + value_->mangle());
}

bool elang::vm::pointer_type_info::is_same(const type_info &type) const{
	if (!is_same_(type))
		return false;

	return (type.is_pointer() && type.underlying_type()->is_const() == value_->is_const() && type.underlying_type()->is_same(*value_));
}

bool elang::vm::pointer_type_info::is_compatible(const type_info &type) const{
	if (is_ref())
		return is_same(type);
	return (type.is_null_pointer() || ((type.is_pointer() || type.is_array()) && (value_->is_void() || value_->is_compatible(*type.underlying_type()))));
}

bool elang::vm::pointer_type_info::is_pointer() const{
	return true;
}

elang::vm::array_type_info::array_type_info(ptr_type value, size_type count, attribute_type attributes)
	: type_info(attributes), value_(value), count_(count){}

elang::vm::type_info::ptr_type elang::vm::array_type_info::clone(attribute_type attributes) const{
	return std::make_shared<array_type_info>(value_, count_, attributes);
}

elang::vm::type_info::size_type elang::vm::array_type_info::size() const{
	return (value_->size() * count_);
}

elang::vm::type_info *elang::vm::array_type_info::underlying_type() const{
	return value_.get();
}

std::string elang::vm::array_type_info::mangle() const{
	return ("A" + std::to_string(count_) + mangle_attributes() + value_->mangle());
}

bool elang::vm::array_type_info::is_same(const type_info &type) const{
	return (type.is_array() && type.underlying_type()->is_same(*value_));
}

bool elang::vm::array_type_info::is_array() const{
	return true;
}

elang::vm::function_type_info::function_type_info(ptr_type return_type, const ptr_list_type &parameters, attribute_type attributes)
	: type_info(attributes), return_type_(return_type), parameters_(parameters){}

elang::vm::function_type_info::function_type_info(ptr_type return_type, ptr_list_type &&parameters, attribute_type attributes)
	: type_info(attributes), return_type_(return_type), parameters_(std::move(parameters)){}

elang::vm::type_info::ptr_type elang::vm::function_type_info::clone(attribute_type attributes) const{
	return std::make_shared<function_type_info>(return_type_, parameters_, attributes);
}

elang::vm::type_info::size_type elang::vm::function_type_info::size() const{
	return 8u;
}

std::string elang::vm::function_type_info::mangle() const{
	return ("F" + mangle_attributes() + mangle_parameters());
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

elang::vm::type_info::ptr_type elang::vm::function_type_info::return_type() const{
	return return_type_;
}

const elang::vm::function_type_info::ptr_list_type &elang::vm::function_type_info::parameters() const{
	return parameters_;
}

std::string elang::vm::function_type_info::mangle_parameters() const{
	if (parameters_.empty())
		return "v";

	std::string mangled_parameters;
	for (auto parameter : parameters_)
		mangled_parameters += parameter->mangle();

	return mangled_parameters;
}

elang::vm::variadic_type_info::variadic_type_info(ptr_type value, attribute_type attributes)
	: type_info(attributes), value_(value){}

elang::vm::type_info::ptr_type elang::vm::variadic_type_info::clone(attribute_type attributes) const{
	return std::make_shared<variadic_type_info>(value_, attributes);
}

elang::vm::type_info::size_type elang::vm::variadic_type_info::size() const{
	return value_->size();
}

elang::vm::type_info *elang::vm::variadic_type_info::underlying_type() const{
	return value_.get();
}

std::string elang::vm::variadic_type_info::mangle() const{
	return ("N" + mangle_attributes() + value_->mangle());
}

bool elang::vm::variadic_type_info::is_same(const type_info &type) const{
	if (!is_same_(type))
		return false;

	return (type.is_variadic() && type.underlying_type()->is_const() == value_->is_const() && type.underlying_type()->is_same(*value_));
}

bool elang::vm::variadic_type_info::is_compatible(const type_info &type) const{
	return value_->is_compatible(type);
}

bool elang::vm::variadic_type_info::is_variadic() const{
	return true;
}
