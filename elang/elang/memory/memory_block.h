#pragma once

#ifndef ELANG_MEMORY_BLOCK_H
#define ELANG_MEMORY_BLOCK_H

#include <memory>

namespace elang::memory{
	struct block{
		typedef unsigned __int64 uint64_type;
		typedef std::size_t size_type;

		typedef std::unique_ptr<char[]> data_type;

		size_type ref_count;
		uint64_type address;
		size_type size;
		size_type actual_size;
		data_type data;
	};
}

#endif /* !ELANG_MEMORY_BLOCK_H */
