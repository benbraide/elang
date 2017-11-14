#pragma once

#ifndef ELANG_SYSCALL_H
#define ELANG_SYSCALL_H

namespace elang::vm{
	class syscall{
	public:
		enum class code{
			exit,
			read_char,
			write_char,
			read,
			write,
			open,
			close,
		};

		static void entry();

		static void exit();

		static void read(bool is_char);

		static void write(bool is_char);
	};
}

#endif /* !ELANG_SYSCALL_H */
