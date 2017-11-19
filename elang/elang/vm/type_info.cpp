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

std::string elang::vm::type_info::mangle() const{
	return (mangle_attributes_(attributes_) + mangle_());
}

std::string elang::vm::type_info::mangle_as_parameter() const{
	if (ELANG_IS_ANY(attributes_, attribute_type::ref_ | attribute_type::vref))
		return (mangle_attributes_(attributes_) + mangle_());
	return (mangle_attributes_(ELANG_REMOVE_V(attributes_, attribute_type::const_)) + mangle_());
}

std::string elang::vm::type_info::mangle_attributes() const{
	return mangle_attributes_(attributes());
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

bool elang::vm::type_info::is_optional() const{
	return false;
}

bool elang::vm::type_info::is_same_(const type_info &type) const{
	return (is_ref() == type.is_ref() && is_vref() == type.is_vref());
}

bool elang::vm::type_info::is_compatible_(const type_info &type) const{
	return (is_vref() == type.is_vref() && (!is_ref() || type.is_ref()));
}

std::string elang::vm::type_info::mangle_attributes_(attribute_type attributes) const{
	std::string mangled_attributes;
	if (ELANG_IS(attributes, attribute_type::const_))
		mangled_attributes = machine::compiler.mangle(mangle_target::const_);

	if (ELANG_IS(attributes, attribute_type::ref_))
		mangled_attributes += machine::compiler.mangle(mangle_target::ref_);
	else if (ELANG_IS(attributes, attribute_type::vref))
		mangled_attributes += machine::compiler.mangle(mangle_target::vref);

	return mangled_attributes;
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

int elang::vm::primitive_type_info::score(const type_info &type) const{
	if (type.is_vref())//Types must match
		return ((is_vref() && (is_const() || !type.is_const()) && is_same_id_(type)) ? ELANG_TYPE_INFO_MAX_SCORE : ELANG_TYPE_INFO_MIN_SCORE);

	if (is_ref()){//Type must be a ref
		if (!is_const() && (!type.is_ref() || type.is_const()))
			return ELANG_TYPE_INFO_MIN_SCORE;

		if (is_same_id_(type))//Same type
			return (type.is_ref() ? ELANG_TYPE_INFO_MAX_SCORE : (ELANG_TYPE_INFO_MAX_SCORE - 1));

		return ((is_const() && has_conversion_(type)) ? (ELANG_TYPE_INFO_MAX_SCORE - 2) : ELANG_TYPE_INFO_MIN_SCORE);
	}

	if (is_same_id_(type))//Same type
		return (type.is_ref() ? (ELANG_TYPE_INFO_MAX_SCORE - 1) : ELANG_TYPE_INFO_MAX_SCORE);

	return (has_conversion_(type) ? (ELANG_TYPE_INFO_MAX_SCORE - 2) : ELANG_TYPE_INFO_MIN_SCORE);
}

bool elang::vm::primitive_type_info::is_same(const type_info &type) const{
	return (is_same_(type) && is_same_id_(type));
}

bool elang::vm::primitive_type_info::is_same_unmodified(const type_info &type) const{
	return is_same_id_(type);
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

bool elang::vm::primitive_type_info::is_same_id_(const type_info &type) const{
	auto basic_type = dynamic_cast<const primitive_type_info *>(&type);
	return (basic_type != nullptr && basic_type->id_ == id_);
}

bool elang::vm::primitive_type_info::has_conversion_(const type_info &type) const{
	if (is_numeric() && type.is_numeric())
		return true;

	return false;
}

elang::vm::user_type_info::user_type_info(symbol_ptr_type value, attribute_type attributes)
	: type_info(attributes), value_(value){}

elang::vm::type_info::ptr_type elang::vm::user_type_info::clone(attribute_type attributes) const{
	return std::make_shared<user_type_info>(value_, attributes);
}

elang::vm::type_info::size_type elang::vm::user_type_info::size() const{
	return value_->size();
}

int elang::vm::user_type_info::score(const type_info &type) const{
	if (type.is_vref())//Types must match
		return ((is_vref() && (is_const() || !type.is_const()) && is_same_symbol_(type)) ? ELANG_TYPE_INFO_MAX_SCORE : ELANG_TYPE_INFO_MIN_SCORE);

	if (is_ref()){//Type must be a ref
		if (!is_const() && (!type.is_ref() || type.is_const()))
			return ELANG_TYPE_INFO_MIN_SCORE;

		if (is_same_symbol_(type))//Same type
			return (type.is_ref() ? ELANG_TYPE_INFO_MAX_SCORE : (ELANG_TYPE_INFO_MAX_SCORE - 1));

		if (!is_const())//Conversions not allowed
			return (is_base_type_(type) ? (ELANG_TYPE_INFO_MAX_SCORE - 2) : ELANG_TYPE_INFO_MIN_SCORE);

		return (has_conversion_(type) ? (ELANG_TYPE_INFO_MAX_SCORE - 2) : ELANG_TYPE_INFO_MIN_SCORE);
	}

	if (is_same_symbol_(type))//Same type
		return (type.is_ref() ? (ELANG_TYPE_INFO_MAX_SCORE - 1) : ELANG_TYPE_INFO_MAX_SCORE);

	return (has_conversion_(type) ? (ELANG_TYPE_INFO_MAX_SCORE - 2) : ELANG_TYPE_INFO_MIN_SCORE);
}

bool elang::vm::user_type_info::is_same(const type_info &type) const{
	return (is_same_(type) && is_same_symbol_(type));
}

bool elang::vm::user_type_info::is_same_unmodified(const type_info &type) const{
	auto basic_type = dynamic_cast<const user_type_info *>(&type);
	return (basic_type != nullptr && basic_type->value_ == value_);
}

bool elang::vm::user_type_info::is_compatible(const type_info &type) const{
	if (is_ref())
		return is_same(type);
	return type_info::is_compatible(type);//#TODO: Implement
}

std::string elang::vm::user_type_info::mangle_() const{
	return value_->mangle();
}

bool elang::vm::user_type_info::is_same_symbol_(const type_info &type) const{
	auto basic_type = dynamic_cast<const user_type_info *>(&type);
	return (basic_type != nullptr && basic_type->value_ == value_);
}

bool elang::vm::user_type_info::is_base_type_(const type_info &type) const{
	auto basic_type = dynamic_cast<const user_type_info *>(&type);
	if (basic_type == nullptr)
		return false;

	auto class_symbol = dynamic_cast<class_type_symbol_entry *>(basic_type->value_.get());
	return (class_symbol != nullptr && class_symbol->is_base(*value_));
}

bool elang::vm::user_type_info::has_conversion_(const type_info &type) const{
	if (is_base_type_(type))
		return true;

	return false;
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

int elang::vm::pointer_type_info::score(const type_info &type) const{
	if (!type.is_pointer()){
		if (type.is_null_pointer())
			return ((!is_vref() && (is_const() || !is_ref())) ? (ELANG_TYPE_INFO_MAX_SCORE - 2) : ELANG_TYPE_INFO_MIN_SCORE);

		if (type.is_array())
			return (is_same_underlying_types_(type) ? ELANG_TYPE_INFO_MAX_SCORE : ELANG_TYPE_INFO_MIN_SCORE);

		return (has_conversion_(type) ? (ELANG_TYPE_INFO_MAX_SCORE - 2) : ELANG_TYPE_INFO_MIN_SCORE);
	}

	if (type.is_vref())//Types must match
		return ((is_vref() && (is_const() || !type.is_const()) && is_same_underlying_types_(type)) ? ELANG_TYPE_INFO_MAX_SCORE : ELANG_TYPE_INFO_MIN_SCORE);

	if (is_ref()){//Type must be a ref
		if (!is_const() && (!type.is_ref() || type.is_const()))
			return ELANG_TYPE_INFO_MIN_SCORE;

		if (is_same_underlying_types_(type))//Same type
			return (type.is_ref() ? ELANG_TYPE_INFO_MAX_SCORE : (ELANG_TYPE_INFO_MAX_SCORE - 1));

		return ((is_const() && has_conversion_(type)) ? (ELANG_TYPE_INFO_MAX_SCORE - 2) : ELANG_TYPE_INFO_MIN_SCORE);
	}

	if (is_same_underlying_types_(type))//Same type
		return (type.is_ref() ? (ELANG_TYPE_INFO_MAX_SCORE - 1) : ELANG_TYPE_INFO_MAX_SCORE);

	if (type.underlying_type()->is_const() && !value_->is_const())
		return ELANG_TYPE_INFO_MIN_SCORE;

	return (value_->is_void() ? (ELANG_TYPE_INFO_MAX_SCORE - 2) : ELANG_TYPE_INFO_MIN_SCORE);
}

bool elang::vm::pointer_type_info::is_same(const type_info &type) const{
	if (!is_same_(type))
		return false;

	return (type.is_pointer() && type.underlying_type()->is_const() == value_->is_const() && type.underlying_type()->is_same(*value_));
}

bool elang::vm::pointer_type_info::is_same_unmodified(const type_info &type) const{
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

std::string elang::vm::pointer_type_info::mangle_() const{
	return (machine::compiler.mangle(mangle_target::pointer) + value_->mangle());
}

bool elang::vm::pointer_type_info::is_same_underlying_types_(const type_info &type) const{
	auto underlying_type = type.underlying_type();
	if (underlying_type == nullptr || (underlying_type->is_const() && !value_->is_const()))
		return false;

	return underlying_type->is_same_unmodified(*value_);
}

bool elang::vm::pointer_type_info::has_conversion_(const type_info &type) const{
	return false;
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

int elang::vm::array_type_info::score(const type_info &type) const{
	return (is_same(type) ? ELANG_TYPE_INFO_MAX_SCORE : ELANG_TYPE_INFO_MIN_SCORE);
}

bool elang::vm::array_type_info::is_same(const type_info &type) const{
	return (type.is_array() && type.underlying_type()->is_same(*value_));
}

bool elang::vm::array_type_info::is_same_unmodified(const type_info &type) const{
	return (type.is_array() && type.underlying_type()->is_same(*value_));
}

bool elang::vm::array_type_info::is_array() const{
	return true;
}

std::string elang::vm::array_type_info::mangle_() const{
	auto array_symbol = machine::compiler.mangle(mangle_target::array_);
	return (array_symbol + std::to_string(count_) + array_symbol + value_->mangle());
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

int elang::vm::function_type_info::score(const type_info &type) const{
	return (is_same(type) ? ELANG_TYPE_INFO_MAX_SCORE : ELANG_TYPE_INFO_MIN_SCORE);
}

bool elang::vm::function_type_info::is_same(const type_info &type) const{
	auto function_type = dynamic_cast<const function_type_info *>(&type);
	return (function_type != nullptr && is_same_return_type(*function_type) && is_same_parameters(*function_type));
}

bool elang::vm::function_type_info::is_same_unmodified(const type_info &type) const{
	return is_same(type);
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
		mangled_parameters += parameter->mangle_as_parameter();

	return mangled_parameters;
}

std::string elang::vm::function_type_info::mangle_() const{
	return (machine::compiler.mangle(mangle_target::function_) + mangle_parameters());
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

int elang::vm::variadic_type_info::score(const type_info &type) const{
	return value_->score(type);
}

bool elang::vm::variadic_type_info::is_same(const type_info &type) const{
	if (!is_same_(type))
		return false;

	return (type.is_variadic() && type.underlying_type()->is_const() == value_->is_const() && type.underlying_type()->is_same(*value_));
}

bool elang::vm::variadic_type_info::is_same_unmodified(const type_info &type) const{
	return (type.is_variadic() && type.underlying_type()->is_const() == value_->is_const() && type.underlying_type()->is_same(*value_));
}

bool elang::vm::variadic_type_info::is_compatible(const type_info &type) const{
	return value_->is_compatible(type);
}

bool elang::vm::variadic_type_info::is_variadic() const{
	return true;
}

bool elang::vm::variadic_type_info::is_optional() const{
	return true;
}

std::string elang::vm::variadic_type_info::mangle_() const{
	return (machine::compiler.mangle(mangle_target::variadic) + value_->mangle());
}

elang::vm::optional_type_info::optional_type_info(ptr_type value)
	: type_info(attribute_type::nil), value_(value){}

elang::vm::type_info::ptr_type elang::vm::optional_type_info::clone(attribute_type attributes) const{
	return value_->clone(attributes);
}

elang::vm::machine_value_type_id elang::vm::optional_type_info::id() const{
	return value_->id();
}

elang::vm::type_info::primitive_id_type elang::vm::optional_type_info::primitive_id() const{
	return value_->primitive_id();
}

elang::vm::type_info::attribute_type elang::vm::optional_type_info::attributes() const{
	return value_->attributes();
}

elang::vm::type_info::size_type elang::vm::optional_type_info::size() const{
	return value_->size();
}

elang::vm::type_info *elang::vm::optional_type_info::underlying_type() const{
	return value_->underlying_type();
}

std::string elang::vm::optional_type_info::mangle() const{
	return value_->mangle();
}

std::string elang::vm::optional_type_info::mangle_as_parameter() const{
	return value_->mangle_as_parameter();
}

std::string elang::vm::optional_type_info::mangle_attributes() const{
	return value_->mangle_attributes();
}

int elang::vm::optional_type_info::score(const type_info &type) const{
	return value_->score(type);
}

bool elang::vm::optional_type_info::is_same(const type_info &type) const{
	return value_->is_same(type);
}

bool elang::vm::optional_type_info::is_same_unmodified(const type_info &type) const{
	return value_->is_same_unmodified(type);
}

bool elang::vm::optional_type_info::is_same_object(const type_info &type) const{
	return value_->is_same_object(type);
}

bool elang::vm::optional_type_info::is_compatible(const type_info &type) const{
	return value_->is_compatible(type);
}

bool elang::vm::optional_type_info::is_null_pointer() const{
	return value_->is_null_pointer();
}

bool elang::vm::optional_type_info::is_pointer() const{
	return value_->is_pointer();
}

bool elang::vm::optional_type_info::is_array() const{
	return value_->is_array();
}

bool elang::vm::optional_type_info::is_function() const{
	return value_->is_function();
}

bool elang::vm::optional_type_info::is_void() const{
	return value_->is_void();
}

bool elang::vm::optional_type_info::is_bool() const{
	return value_->is_bool();
}

bool elang::vm::optional_type_info::is_numeric() const{
	return value_->is_numeric();
}

bool elang::vm::optional_type_info::is_integral() const{
	return value_->is_integral();
}

bool elang::vm::optional_type_info::is_const() const{
	return value_->is_const();
}

bool elang::vm::optional_type_info::is_ref() const{
	return value_->is_ref();
}

bool elang::vm::optional_type_info::is_vref() const{
	return value_->is_vref();
}

bool elang::vm::optional_type_info::is_variadic() const{
	return value_->is_variadic();
}

bool elang::vm::optional_type_info::is_optional() const{
	return true;
}

std::string elang::vm::optional_type_info::mangle_() const{
	return "";
}
