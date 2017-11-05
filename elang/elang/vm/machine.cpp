#include "machine.h"

void elang::vm::machine::boot(){
	
}

void elang::vm::machine::shutdown(bool thread){
	if (thread)
		running_thread = false;
	else//Exit
		running = false;
}

void elang::vm::machine::run(){
	if (running_thread)
		return;//Already running

	if (cached_registers.instruction_pointer->read_64bits() == 0u){//Find start label
		auto &label = asm_translation.start_label();
		auto start_address = asm_translation.find_address((label.empty() ? start_label : label), std::vector<std::string>{});

		if (start_address != static_cast<uint64_type>(-1))//Load address
			cached_registers.instruction_pointer->write(start_address);
		else//Could not find start label
			throw error_type::no_start_label;
	}

	running = running_thread = true;
	runtime.stack.create(stack_size);

	while (running && running_thread){
		auto next_instruction = asm_translation.find(cached_registers.instruction_pointer->read_64bits());
		if (next_instruction != nullptr)
			next_instruction->execute_and_update_instruction_pointer(*cached_registers.instruction_pointer);
		else//Error
			throw error_type::no_instruction;
	}
}

elang::vm::machine_register_manager elang::vm::machine::register_manager;

elang::vm::machine::memory_manager_type elang::vm::machine::memory_manager;

elang::vm::machine::asm_translation_type elang::vm::machine::asm_translation;

elang::vm::machine::runtime_info_type elang::vm::machine::runtime;

thread_local elang::vm::machine::cached_register_info elang::vm::machine::cached_registers{};

elang::vm::machine::memory_manager_type::range_type elang::vm::machine::access_protected{};

elang::vm::machine::size_type elang::vm::machine::stack_size = (1024 * 1024);//Default size is 1MB

const std::string elang::vm::machine::start_label = "_start";

bool elang::vm::machine::running = false;

thread_local bool elang::vm::machine::running_thread = false;
