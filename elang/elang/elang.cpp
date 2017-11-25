#include "grammar/parser/declaration_parser.h"

int main(){
	std::string input;

	std::cout << "Enter File: ";
	std::cin >> input;

	while (input != "q" && input != "Q"){
		elang::vm::asm_translation::translate(input);
		std::cout << "Enter File(s): ";
		std::cin >> input;
	}

	return 0;
}
