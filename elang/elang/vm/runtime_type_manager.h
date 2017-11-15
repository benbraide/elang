#pragma once

#ifndef ELANG_RUNTIME_TYPE_MANAGER_H
#define ELANG_RUNTIME_TYPE_MANAGER_H

#include "runtime_type.h"

namespace elang::vm{
	class runtime_type_manager{
	public:
		typedef runtime_type::ptr_type runtime_ptr_type;
		typedef std::unordered_map<std::string, runtime_ptr_type> map_type;

		void boot(){
			map_.clear();
		}

		runtime_type *find(const std::string &key) const{
			auto entry = map_.find(key);
			return ((entry == map_.end()) ? nullptr : entry->second.get());
		}

		void add(const std::string &key, runtime_ptr_type value){
			if (map_.find(key) == map_.end())
				map_[key] = value;
			else//Error
				throw runtime_error::type_redefinition;
		}

	private:
		map_type map_;
	};
}

#endif /* !ELANG_RUNTIME_TYPE_MANAGER_H */
