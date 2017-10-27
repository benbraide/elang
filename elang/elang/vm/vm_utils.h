#pragma once

#ifndef ELANG_VM_UTILS_H
#define ELANG_VM_UTILS_H

#include <string>

namespace elang{
	namespace vm{
		class utils{
		public:
			static void to_lower(std::string &buffer);
		};
	}
}

#endif /* !ELANG_VM_UTILS_H */
