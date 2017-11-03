#pragma once

#ifndef ELANG_RUNTIME_H
#define ELANG_RUNTIME_H

#include "runtime_stack.h"
#include "runtime_type_manager.h"

namespace elang{
	namespace vm{
		class runtime{
		public:
			typedef runtime_error error_type;
			typedef elang::vm::runtime_type_manager runtime_type_manager_type;

			static void enter_protected_mode();

			static void leave_protected_mode();

			static bool is_inside_protected_mode();

			static thread_local unsigned int protected_mode_count;

			static thread_local runtime_stack stack;

			static runtime_type_manager_type runtime_type_manager;
		};
	}
}

#endif /* !ELANG_RUNTIME_H */
