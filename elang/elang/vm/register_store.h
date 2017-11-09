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
		typedef machine_value_type_id value_type_id_type;

		struct info_type{
			machine_register *qword;
			machine_register *dword;
			machine_register *word;
			machine_register *byte;
		};

		typedef std::vector<info_type> cache_list_type;
		typedef std::vector<info_type *> info_list_type;
		typedef std::vector<machine_register *> raw_info_list_type;

		machine_register *get(value_type_id_type value_type);

		machine_register *get_used(value_type_id_type value_type);

	private:
		void init_();

		machine_register *get_(value_type_id_type value_type);

		cache_list_type cache_;
		info_list_type available_list_;
		raw_info_list_type available_float_list_;
		raw_info_list_type used_list_;
		info_list_type integral_used_list_;
		bool initialized_ = false;
	};
}

#endif /* !ELANG_REGISTER_STORE_H */
