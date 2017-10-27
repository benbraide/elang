#pragma once

#ifndef ELANG_RUNTIME_STACK_H
#define ELANG_RUNTIME_STACK_H

#include <memory>

#include "runtime_stack_error.h"

namespace elang{
	namespace vm{
		class runtime_stack{
		public:
			typedef unsigned __int64 uint64_type;
			typedef std::size_t size_type;

			typedef runtime_stack_error error_type;

			explicit runtime_stack(size_type data_size = 0u);

			void create(size_type data_size = 0u);

			void push(size_type size);

			void push(const char *value, size_type size);

			template <typename value_type>
			void push(value_type value){
				push(reinterpret_cast<char *>(&value), sizeof(value_type));
			}

			void pop(size_type size);

			void pop(char *value, size_type size);

			template <typename value_type>
			value_type pop(){
				auto value = value_type();
				pop(reinterpret_cast<char *>(&value), sizeof(value_type));
				return value;
			}

		private:
			uint64_type address_;
			char *data_;
			char *max_;
		};
	}
}

#endif /* !ELANG_RUNTIME_STACK_H */
