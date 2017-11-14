#pragma once

#ifndef ELANG_SYSCALL_H
#define ELANG_SYSCALL_H

namespace elang::vm{
	class syscall{
	public:
		enum class code{
			exit,
			read,
			write_char,
			write,
			open,
			close,
		};

		static void entry();

		static void exit();

		static void read();

		static void write(bool is_char);
	};
}

#endif /* !ELANG_SYSCALL_H */
