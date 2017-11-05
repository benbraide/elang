#pragma once

#ifndef ELANG_MEMORY_MANAGER_H
#define ELANG_MEMORY_MANAGER_H

#include <map>
#include <memory>
#include <shared_mutex>
#include <unordered_map>

#include "memory_error.h"
#include "memory_block.h"

namespace elang::memory{
	class manager{
	public:
		typedef error error_type;
		typedef block block_type;

		typedef block_type::uint64_type uint64_type;
		typedef block_type::size_type size_type;
		typedef block_type::data_type data_type;

		struct range_type{
			uint64_type from;
			uint64_type to;
		};

		typedef std::unordered_map<uint64_type, block_type> map_type;
		typedef std::map<uint64_type, size_type> size_map_type;
		typedef map_type::iterator map_iterator_type;

		typedef std::shared_mutex lock_type;
		typedef std::lock_guard<lock_type> lock_guard_type;
		typedef std::shared_lock<lock_type> shared_lock_guard_type;

		void protect_from_access(const range_type &range);

		bool is_access_protected(const range_type &range) const;

		void protect_from_write(const range_type &range);

		bool is_write_protected(const range_type &range) const;

		void deallocate(uint64_type address);

		uint64_type reserve(size_type size);

		block_type *allocate(size_type size, uint64_type address_hint = 0ull);

		block_type *allocate_contiguous(size_type count, size_type size);

		block_type *reallocate(uint64_type address, size_type size);

		void fill(uint64_type address, char source, size_type count);

		void copy(uint64_type destination, uint64_type source, size_type size);

		template <typename value_type>
		void write(uint64_type destination, value_type value){
			return write(destination, reinterpret_cast<char *>(&value), sizeof(value_type));
		}

		template <typename value_type>
		void write(uint64_type destination, const value_type *value, size_type count){
			return write(destination, reinterpret_cast<const char *>(value), sizeof(value_type) * count);
		}

		void write(uint64_type destination, const char *source, size_type size);

		template <typename target_type>
		target_type read(uint64_type source){
			auto value = target_type();
			read(source, (char *)&value, sizeof(target_type));
			return value;
		}

		void read(uint64_type source, char *destination, size_type size);

		block_type *find(uint64_type address);

		block_type *find_nearest(uint64_type address);

	private:
		void add_available_(uint64_type value, size_type size);

		uint64_type find_available_(size_type size, uint64_type match = 0ull);

		void deallocate_(uint64_type address, bool add_to_available);

		uint64_type reserve_(size_type size);

		block_type *allocate_block_(size_type size, uint64_type address);

		block_type *allocate_(size_type size, uint64_type address_hint);

		block_type *allocate_contiguous_(size_type count, size_type size);

		block_type *reallocate_(uint64_type address, size_type size);

		void fill_(uint64_type address, char source, size_type count);

		void copy_(uint64_type destination, uint64_type source, size_type size);

		void write_(uint64_type destination, const char *source, size_type size, bool is_array);

		void read_(uint64_type source, char *destination, size_type size);

		map_iterator_type find_raw_(uint64_type address);

		block_type *find_(uint64_type address);

		block_type *find_nearest_(uint64_type address);

		range_type access_protected_{};
		range_type write_protected_{};
		uint64_type next_address_ = 1u;
		map_type map_;
		size_map_type available_;
		lock_type lock_;
	};
}

#endif /* !ELANG_MEMORY_MANAGER_H */
