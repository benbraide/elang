#pragma once

#ifndef ELANG_RUNTIME_ERROR_H
#define ELANG_RUNTIME_ERROR_H

namespace elang::vm{
	enum class runtime_error{
		nil,
		not_inside_protected_mode,
		type_redefinition,
	};
}

#endif /* !ELANG_RUNTIME_ERROR_H */
