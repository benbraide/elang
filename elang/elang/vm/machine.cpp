#include "machine.h"

void elang::vm::machine::boot(){
	memory_manager.boot();
	asm_translation.boot();

	running = running_thread = false;
	cached_registers.instruction_pointer->write(static_cast<uint64_type>(0u));

	runtime.protected_mode_count = 0u;
	runtime.stack.boot();
	runtime.runtime_type_manager.boot();
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
	runtime.stack.create((asm_translation.stack_size() == 0u) ? stack_size : asm_translation.stack_size());

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

elang::vm::machine::syscall_register_store_type elang::vm::machine::syscall_register_store;

elang::vm::machine::asm_translation_type elang::vm::machine::asm_translation;

elang::vm::machine::runtime_info_type elang::vm::machine::runtime;

thread_local elang::vm::machine::cached_register_info elang::vm::machine::cached_registers{};

elang::vm::machine::memory_manager_type::range_type elang::vm::machine::access_protected{};

elang::vm::machine::compiler_type elang::vm::machine::compiler;

elang::vm::machine::size_type elang::vm::machine::stack_size = (1024 * 1024);//Default size is 1MB

const std::string elang::vm::machine::start_label = "_start";

elang::vm::machine::ostream_output_writer_type elang::vm::machine::out_writer(std::cout, std::wcout);

elang::vm::machine::ostream_output_writer_type elang::vm::machine::err_writer(std::cerr, std::wcerr);

elang::vm::machine::ostream_output_writer_type elang::vm::machine::log_writer(std::clog, std::wclog);

elang::vm::machine::ostream_output_writer_type *elang::vm::machine::current_writer = &out_writer;

elang::vm::machine::istream_input_reader_type elang::vm::machine::in_reader(std::cin, std::wcin);

elang::vm::machine::istream_input_reader_type *elang::vm::machine::current_reader = &in_reader;

bool elang::vm::machine::running = false;

thread_local bool elang::vm::machine::running_thread = false;
