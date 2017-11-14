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
	case code::open:
		break;
	case code::close:
		break;
	default:
		break;
	}
}

void elang::vm::syscall::exit(){
	machine::shutdown(false);
}

void elang::vm::syscall::read(){
	auto size = machine::register_manager.find("rcx")->read_64bits();
	if (size != 0u)
		machine::memory_manager.write(machine::register_manager.find("rbx")->read_64bits(), *machine::current_reader, size);
}

void elang::vm::syscall::write(bool is_char){
	if (!is_char){//Write buffer
		auto size = machine::register_manager.find("rcx")->read_64bits();
		if (size != 0u)
			machine::memory_manager.read(machine::register_manager.find("rbx")->read_64bits(), *machine::current_writer, size);
	}
	else//Write char
		elang::vm::machine::out_writer.write_char(static_cast<char>(machine::register_manager.find("rbx")->read_64bits()));
}
