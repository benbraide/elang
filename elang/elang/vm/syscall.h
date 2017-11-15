#pragma once

#ifndef ELANG_SYSCALL_H
#define ELANG_SYSCALL_H

namespace elang::vm{
	class syscall{
	public:
		enum class error_type{
			nil,
			no_output_writer,
			no_input_reader,
			not_heap_memory,
		};

		enum class code{
			exit,
			read,
			write_char,
			write,
			write_int,
			write_float,
			open,
			close,
			read_file,
			write_file,
			allocate			= 0x20,
			reallocate,
			deallocate,
			mem_set,
			mem_copy,
			begin_thread		= 0x28,
			exit_thread,
			get_thread_id,
		};

		static void entry();

		static void exit();

		static void read();

		static void write(bool is_char);

		static void write_integer();

		static void write_float();

		static void allocate();

		static void reallocate();

		static void deallocate();

		static void mem_set();

		static void mem_copy();
	};
}

#endif /* !ELANG_SYSCALL_H */
