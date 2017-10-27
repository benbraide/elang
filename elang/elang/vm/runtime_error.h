#pragma once

#ifndef ELANG_RUNTIME_ERROR_H
#define ELANG_RUNTIME_ERROR_H

namespace elang{
	namespace vm{
		enum class runtime_error{
			nil,
			not_inside_protected_mode,
		};
	}
}

#endif /* !ELANG_RUNTIME_ERROR_H */
