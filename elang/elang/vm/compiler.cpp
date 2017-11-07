#include "machine.h"
#include "compiler.h"

elang::vm::compiler::compiler(){
	register_list_.reserve(4u);
}

elang::vm::register_store &elang::vm::compiler::store(){
	return store_;
}

void elang::vm::compiler::push_register(machine_register &reg){
	for (auto entry = register_list_.begin(); entry != register_list_.end(); ++entry){
		if (*entry == nullptr){
			*entry = &reg;
			return;
		}
	}

	throw machine_error::no_register;
}

elang::vm::machine_register *elang::vm::compiler::pop_register(){
	if (register_list_[0] == nullptr)
		return nullptr;

	auto reg = register_list_[0];
	auto entry = register_list_.begin();

	for (auto next = std::next(entry); next != register_list_.end(); ++entry, ++next){
		if (*entry != nullptr)
			*entry = *next;
		else//No used spot
			break;
	}

	return reg;
}
