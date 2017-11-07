#pragma once

#ifndef ELANG_COMPILER_H
#define ELANG_COMPILER_H

#include "../asm/instruction_section.h"

#include "register_store.h"

namespace elang::vm{
	class compiler{
	public:
		typedef std::vector<machine_register *> register_list_type;

		compiler();

		register_store &store();

		void push_register(machine_register &reg);

		machine_register *pop_register();

	private:
		register_store store_;
		register_list_type register_list_;
	};
}

#endif /* !ELANG_COMPILER_H */
