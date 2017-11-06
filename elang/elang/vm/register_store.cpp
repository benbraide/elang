#include "machine.h"
#include "register_store.h"

elang::vm::syscall_register_store::syscall_register_store(){
	list_.reserve(13);
	float_list_.reserve(8);

	list_.push_back("rax");
	list_.push_back("rbx");
	list_.push_back("rcx");
	list_.push_back("rdx");
	list_.push_back("rsi");
	list_.push_back("rdi");

	list_.push_back("r9");
	list_.push_back("r10");
	list_.push_back("r11");
	list_.push_back("r12");
	list_.push_back("r13");
	list_.push_back("r14");
	list_.push_back("r15");

	float_list_.push_back("xmm0");
	float_list_.push_back("xmm1");
	float_list_.push_back("xmm2");
	float_list_.push_back("xmm3");
	float_list_.push_back("xmm4");
	float_list_.push_back("xmm5");
	float_list_.push_back("xmm6");
	float_list_.push_back("xmm7");
}

elang::vm::machine_register *elang::vm::syscall_register_store::get(unsigned int index, bool float_){
	auto &list = (float_ ? float_list_ : list_);
	return ((list.size() <= index) ? nullptr : machine::register_manager.find(list[index]));
}
