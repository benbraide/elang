#pragma once

#ifndef ELANG_MACHINE_REGISTER_ERROR_H
#define ELANG_MACHINE_REGISTER_ERROR_H

namespace elang{
	namespace vm{
		enum class machine_register_error{
			nil,
			indivisible_size,
			size_mismatch,
			type_mismatch,
			not_found,
		};
	}
}

#endif /* !ELANG_MACHINE_REGISTER_ERROR_H */
