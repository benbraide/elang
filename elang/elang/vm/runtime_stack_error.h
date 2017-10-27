#pragma once

#ifndef ELANG_RUNTIME_STACK_ERROR_H
#define ELANG_RUNTIME_STACK_ERROR_H

namespace elang{
	namespace vm{
		enum class runtime_stack_error{
			nil,
			overflow,
			underflow,
		};
	}
}

#endif /* !ELANG_RUNTIME_STACK_ERROR_H */
