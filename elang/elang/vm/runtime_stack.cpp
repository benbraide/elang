#include "machine.h"

elang::vm::runtime_stack::runtime_stack(size_type data_size)
	: address_(0), data_(nullptr), max_(nullptr){
	if (data_size != 0u)//Allocate memory
		create(data_size);
}

void elang::vm::runtime_stack::create(size_type data_size){
	if (data_ != nullptr)
		return;//Already allocated

	if (data_size == 0u)//Use size in VM
		data_size = machine::stack_size;

	auto block = machine::memory_manager.allocate(data_size);
	machine::cached_registers.stack_pointer->write(address_ = block->address);

	data_ = block->data.get();
	max_ = (data_ + data_size);
}

void elang::vm::runtime_stack::push(size_type size){
	push(nullptr, size);
}

void elang::vm::runtime_stack::push(const char *value, size_type size){
	auto current_address = machine::cached_registers.stack_pointer->read<uint64_type>();
	auto next_address = (current_address + size);//Address after write

	if ((data_ + (next_address - address_)) > max_)
		throw error_type::overflow;//Overflow

	if (value != nullptr)//Copy bytes
		memcpy((data_ + current_address), value, size);

	machine::cached_registers.stack_pointer->write(next_address);//Update register value
}

void elang::vm::runtime_stack::push(uint64_type address, size_type size){
	auto current_address = machine::cached_registers.stack_pointer->read<uint64_type>();
	auto next_address = (current_address + size);//Address after write

	if ((data_ + (next_address - address_)) > max_)
		throw error_type::overflow;//Overflow

	machine::memory_manager.read(address, (data_ + current_address), size);
	machine::cached_registers.stack_pointer->write(next_address);//Update register value
}

void elang::vm::runtime_stack::pop(size_type size){
	pop(nullptr, size);
}

void elang::vm::runtime_stack::pop(char *value, size_type size){
	auto current_address = machine::cached_registers.stack_pointer->read<uint64_type>();
	if ((current_address - address_) < size)
		throw error_type::underflow;//Error

	if (value != nullptr)//Copy bytes
		memcpy(value, (data_ + (current_address - size)), size);

	machine::cached_registers.stack_pointer->write(current_address - size);//Update register value
}

void elang::vm::runtime_stack::pop(uint64_type address, size_type size){
	auto current_address = machine::cached_registers.stack_pointer->read<uint64_type>();
	if ((current_address - address_) < size)
		throw error_type::underflow;//Error

	machine::memory_manager.write(address, (data_ + (current_address - size)), size);
	machine::cached_registers.stack_pointer->write(current_address - size);//Update register value
}
