#include "machine.h"

elang::vm::machine_register_manager elang::vm::machine::register_manager;

elang::vm::machine::memory_manager_type elang::vm::machine::memory_manager;

elang::vm::machine::asm_translation_type elang::vm::machine::asm_translation;

elang::vm::machine::runtime_info_type elang::vm::machine::runtime;

thread_local elang::vm::machine::cached_register_info elang::vm::machine::cached_registers{};

elang::vm::machine::size_type elang::vm::machine::stack_size = (1024 * 1024);//Default size is 1MB

elang::vm::machine::memory_manager_type::range_type elang::vm::machine::access_protected{};
