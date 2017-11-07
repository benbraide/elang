#pragma once

#ifndef ELANG_MACHINE_H
#define ELANG_MACHINE_H

#include "../memory/memory_manager.h"

#include "runtime.h"
#include "machine_register_manager.h"
#include "compiler.h"
#include "asm_translation.h"

namespace elang::vm{
	enum class machine_error{
		nil,
		no_start_label,
		no_instruction,
		no_register,
	};

	class machine{
	public:
		typedef unsigned __int64 uint64_type;
		typedef std::size_t size_type;

		typedef machine_error error_type;

		typedef elang::vm::syscall_register_store syscall_register_store_type;
		typedef elang::vm::compiler compiler_type;

		typedef elang::memory::manager memory_manager_type;
		typedef elang::vm::asm_translation asm_translation_type;
		typedef elang::vm::runtime runtime_info_type;

		struct cached_register_info{
			machine_register *instruction_pointer;
			machine_register *stack_pointer;
			machine_register *base_pointer;
			machine_register *compare_target;
		};

		static void boot();

		static void shutdown(bool thread);

		static void run();

		static machine_register_manager register_manager;
		static memory_manager_type memory_manager;

		static syscall_register_store_type syscall_register_store;
		static compiler_type compiler;

		static asm_translation_type asm_translation;
		static runtime_info_type runtime;

		static thread_local cached_register_info cached_registers;
		static memory_manager_type::range_type access_protected;

		static size_type stack_size;
		static const std::string start_label;

		static bool running;
		static thread_local bool running_thread;
	};
}

#endif /* !ELANG_MACHINE_H */
