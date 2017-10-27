#pragma once

#ifndef ELANG_MACHINE_H
#define ELANG_MACHINE_H

#include "../memory/memory_manager.h"

#include "runtime.h"
#include "machine_register_manager.h"

namespace elang{
	namespace vm{
		class machine{
		public:
			typedef unsigned __int64 uint64_type;
			typedef std::size_t size_type;

			typedef elang::memory::manager memory_manager_type;
			typedef elang::vm::runtime runtime_type;

			struct cached_register_info{
				machine_register *instruction_pointer;
				machine_register *stack_pointer;
				machine_register *base_pointer;
				machine_register *compare_target;
			};

			static machine_register_manager register_manager;
			static memory_manager_type memory_manager;

			static runtime_type runtime;
			static thread_local cached_register_info cached_registers;
			static size_type stack_size;
		};
	}
}

#endif /* !ELANG_MACHINE_H */
