#pragma once

#ifndef ELANG_MEMORY_ERROR_H
#define ELANG_MEMORY_ERROR_H

namespace elang{
	namespace memory{
		enum class error{
			nil,
			out_of_memory,
			invalid_address,
			access_violation,
			write_access_violation,
			invalid_count,
		};
	}
}

#endif /* !ELANG_MEMORY_ERROR_H */
