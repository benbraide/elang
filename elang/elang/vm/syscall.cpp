#include "machine.h"
#include "syscall.h"

void elang::vm::syscall::entry(){
	switch (static_cast<code>(machine::register_manager.find("rax")->read_64bits())){
	case code::exit:
		exit();
		break;
	case code::read:
		read();
		break;
	case code::write_char:
		write(true);
		break;
	case code::write:
		write(false);
		break;
	case code::write_int:
		write_integer();
		break;
	case code::write_float:
		write_float();
		break;
	case code::open:
		break;
	case code::close:
		break;
	case code::allocate:
		allocate();
		break;
	case code::reallocate:
		reallocate();
		break;
	case code::deallocate:
		deallocate();
		break;
	case code::mem_set:
		mem_set();
		break;
	case code::mem_copy:
		mem_copy();
		break;
	default:
		break;
	}
}

void elang::vm::syscall::exit(){
	machine::shutdown(false);
}

void elang::vm::syscall::read(){
	if (machine::current_reader == nullptr)
		throw error_type::no_input_reader;

	auto size = machine::register_manager.find("rcx")->read_64bits();
	if (size != 0u)
		machine::memory_manager.write(machine::register_manager.find("rbx")->read_64bits(), *machine::current_reader, size);
}

void elang::vm::syscall::write(bool is_char){
	if (machine::current_writer == nullptr)
		throw error_type::no_output_writer;

	if (!is_char){//Write buffer
		auto size = machine::register_manager.find("rcx")->read_64bits();
		if (size != 0u)
			machine::memory_manager.read(machine::register_manager.find("rbx")->read_64bits(), *machine::current_writer, size);
	}
	else//Write char
		elang::vm::machine::out_writer.write_char(static_cast<char>(machine::register_manager.find("rbx")->read_64bits()));
}

void elang::vm::syscall::write_integer(){
	if (machine::current_writer != nullptr)
		elang::vm::machine::out_writer.write(machine::register_manager.find("rbx")->read_64bits());
	else//Error
		throw error_type::no_output_writer;
}

void elang::vm::syscall::write_float(){
	if (machine::current_writer != nullptr)
		elang::vm::machine::out_writer.write(machine::register_manager.find("xmm0")->read<long double>());
	else//Error
		throw error_type::no_output_writer;
}

void elang::vm::syscall::allocate(){
	auto block = machine::memory_manager.allocate(machine::register_manager.find("rbx")->read_64bits());
	block->ref_count = static_cast<std::size_t>(-1);//Indicate heap memory
	machine::register_manager.find("rax")->write(block->address);
}

void elang::vm::syscall::reallocate(){
	auto block = machine::memory_manager.find(machine::register_manager.find("rbx")->read_64bits());
	if (block == nullptr || block->ref_count != static_cast<std::size_t>(-1))
		throw error_type::not_heap_memory;

	block->ref_count = 1u;
	block = machine::memory_manager.reallocate(block->address, machine::register_manager.find("rcx")->read_64bits());
	block->ref_count = static_cast<std::size_t>(-1);

	machine::register_manager.find("rax")->write(block->address);
}

void elang::vm::syscall::deallocate(){
	auto block = machine::memory_manager.find(machine::register_manager.find("rbx")->read_64bits());
	if (block != nullptr && block->ref_count == static_cast<std::size_t>(-1)){
		block->ref_count = 1u;
		machine::memory_manager.deallocate(block->address);
	}
	else//Error
		throw error_type::not_heap_memory;
}

void elang::vm::syscall::mem_set(){
	machine::memory_manager.fill(
		machine::register_manager.find("rbx")->read_64bits(),
		static_cast<char>(machine::register_manager.find("rdx")->read_64bits()),
		machine::register_manager.find("rcx")->read_64bits()
	);
}

void elang::vm::syscall::mem_copy(){
	machine::memory_manager.copy(
		machine::register_manager.find("rbx")->read_64bits(),
		machine::register_manager.find("rcx")->read_64bits(),
		machine::register_manager.find("rdx")->read_64bits()
	);
}
