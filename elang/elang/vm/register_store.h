#pragma once

#ifndef ELANG_REGISTER_STORE_H
#define ELANG_REGISTER_STORE_H

#include <vector>

#include "machine_register.h"

namespace elang::vm{
	class syscall_register_store{
	public:
		typedef std::vector<std::string> list_type;

		syscall_register_store();

		machine_register *get(unsigned int index, bool float_);

	private:
		list_type list_;
		list_type float_list_;
	};
}

#endif /* !ELANG_REGISTER_STORE_H */
