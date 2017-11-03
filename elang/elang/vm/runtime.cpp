#include "runtime.h"

void elang::vm::runtime::enter_protected_mode(){
	++protected_mode_count;
}

void elang::vm::runtime::leave_protected_mode(){
	if (protected_mode_count == 0u)
		throw error_type::not_inside_protected_mode;
	else//Decrement count
		--protected_mode_count;
}

bool elang::vm::runtime::is_inside_protected_mode(){
	return (protected_mode_count > 0u);
}

thread_local unsigned int elang::vm::runtime::protected_mode_count = 0u;

thread_local elang::vm::runtime_stack elang::vm::runtime::stack;

elang::vm::runtime::runtime_type_manager_type elang::vm::runtime::runtime_type_manager;
