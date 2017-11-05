#include "machine.h"
#include "syscall.h"

void elang::vm::syscall::entry(){
	switch (static_cast<code>(machine::register_manager.find("rax")->read_64bits())){
	case code::exit:
		exit();
		break;
	case code::read:
		break;
	case code::write:
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
