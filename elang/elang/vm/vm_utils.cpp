#include "vm_utils.h"

void elang::vm::utils::to_lower(std::string &buffer){
	for (auto &c : buffer)
		c = tolower(c);
}
