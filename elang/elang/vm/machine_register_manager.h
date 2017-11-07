#pragma once

#ifndef ELANG_MACHINE_REGISTER_MANAGER_H
#define ELANG_MACHINE_REGISTER_MANAGER_H

#include <unordered_map>

#include "vm_utils.h"
#include "machine_register.h"

namespace elang::vm{
	class machine_register_manager{
	public:
		typedef machine_register_error error_type;
		typedef machine_register::ptr_type register_ptr_type;

		typedef std::unordered_map<std::string, register_ptr_type> map_type;

		machine_register_manager();

		machine_register *find(std::string key) const;

		const map_type &content() const;

	private:
		void add_(const std::string &name, const std::string &alias, const std::string &_32, const std::string &_16, const std::string &low, const std::string &high);

		void add_qword_(const std::string &prefix, int from, int to);

		template <typename value_type>
		void add_float_(const std::string &prefix, int from, int to){
			std::string name;
			for (; from <= to; ++from){//Add entries
				name = (prefix + std::to_string(from));
				map_[name] = std::make_shared<basic_machine_register<value_type>>(name);
			}
		}

		map_type map_;
	};
}

#endif /* !ELANG_MACHINE_REGISTER_MANAGER_H */
