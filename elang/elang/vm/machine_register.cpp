#include "machine.h"

elang::vm::machine_register::machine_register(const std::string &name)
	: name_(name){}

elang::vm::machine_register::machine_register(const std::string &name, size_type size, char *buffer)
	: name_(name), size_(size), buffer_(buffer){}

elang::vm::machine_register::value_type_id_type elang::vm::machine_register::type_id() const{
	switch (size_){
	case 1u:
		return value_type_id_type::byte;
	case 2u:
		return value_type_id_type::word;
	case 4u:
		return value_type_id_type::dword;
	case 8u:
		return value_type_id_type::qword;
	default:
		break;
	}

	throw error_type::size_mismatch;
}

elang::vm::machine_register::uint64_type elang::vm::machine_register::read_64bits() const{
	switch (size_){
	case 1u:
		return static_cast<uint64_type>(read<__int8>());
	case 2u:
		return static_cast<uint64_type>(read<__int16>());
	case 4u:
		return static_cast<uint64_type>(read<__int32>());
	default:
		break;
	}

	return read<uint64_type>();
}

elang::vm::machine_register::ptr_type elang::vm::machine_register::alias(const std::string &name) const{
	return std::make_shared<machine_register>(name, size_, buffer_);
}

elang::vm::machine_register::ptr_type elang::vm::machine_register::low(const std::string &name) const{
	if (size_ < 2u)
		throw error_type::indivisible_size;
	return std::make_shared<machine_register>(name, (size_ / 2u), buffer_);
}

elang::vm::machine_register::ptr_type elang::vm::machine_register::high(const std::string &name) const{
	if (size_ < 2u)
		throw error_type::indivisible_size;
	return std::make_shared<machine_register>(name, (size_ / 2u), (buffer_ + (size_ / 2u)));
}

void elang::vm::machine_register::push_onto_stack(){
	machine::runtime.stack.push(buffer_, size_);
}

void elang::vm::machine_register::pop_from_stack(){
	machine::runtime.stack.pop(buffer_, size_);
}

const std::string &elang::vm::machine_register::name() const{
	return name_;
}

elang::vm::machine_register::size_type elang::vm::machine_register::size() const{
	return size_;
}

void elang::vm::machine_register::init_(size_type size, char *buffer){
	size_ = size;
	buffer_ = buffer;
}
