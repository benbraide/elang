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

elang::vm::machine_register *elang::vm::register_store::get(size_type size){
	init_();
	for (auto &entry : map_[size]){
		if (entry.second){//Unused
			entry.second = false;
			return entry.first;
		}
	}

	return nullptr;
}

elang::vm::machine_register *elang::vm::register_store::get_float(){
	init_();
	for (auto &entry : float_list_){
		if (entry.second){//Unused
			entry.second = false;
			return entry.first;
		}
	}

	return nullptr;
}

void elang::vm::register_store::put(machine_register &reg){
	map_[reg.size()][&reg] = true;//Restore
}

void elang::vm::register_store::init_(){
	if (external_map_ != nullptr)
		return;//Already initialized

	external_map_ = &machine::register_manager.content();
	for (auto &entry : *external_map_){
		if (entry.second->type_id() == machine_value_type_id::float_)//Float register
			float_list_[entry.second.get()] = true;
		else//Integral register
			map_[entry.second->size()][entry.second.get()] = true;
	}
}
