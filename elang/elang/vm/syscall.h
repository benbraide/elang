#pragma once

#ifndef ELANG_SYSCALL_H
#define ELANG_SYSCALL_H

namespace elang::vm{
	class syscall{
	public:
		enum class code{
			exit,
			read,
			write,
			open,
			close,
		};

		static void entry();

		static void exit();
	};
}

#endif /* !ELANG_SYSCALL_H */
