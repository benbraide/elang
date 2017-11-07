#pragma once

#ifndef ELANG_REGISTER_STORE_H
#define ELANG_REGISTER_STORE_H

#include <vector>
#include <unordered_map>

#include "machine_register_manager.h"

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

	class register_store{
	public:
		typedef std::size_t size_type;
		typedef machine_register_manager::map_type external_map_type;

		typedef std::unordered_map<machine_register *, bool> list_type;
		typedef std::unordered_map<size_type, list_type> map_type;

		machine_register *get(size_type size);

		machine_register *get_float();

		void put(machine_register &reg);

	private:
		void init_();

		map_type map_;
		list_type float_list_;
		const external_map_type *external_map_ = nullptr;
	};
}

#endif /* !ELANG_REGISTER_STORE_H */
