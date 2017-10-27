#pragma once

#ifndef ELANG_RUNTIME_H
#define ELANG_RUNTIME_H

#include "runtime_error.h"

namespace elang{
	namespace vm{
		class runtime{
		public:
			typedef runtime_error error_type;

			static void enter_protected_mode();

			static void leave_protected_mode();

			static bool is_inside_protected_mode();

			static thread_local unsigned int protected_mode_count;
		};
	}
}

#endif /* !ELANG_RUNTIME_H */
