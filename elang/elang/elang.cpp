#include "grammar/parser/expression_parser.h"

int main(){
	namespace x3 = boost::spirit::x3;
	namespace ast = elang::grammar::ast;
	namespace parser = elang::grammar::parser;
	namespace vm = elang::vm;
	namespace common = elang::common;

	vm::asm_translation::translate("hello_world");
	//vm::asm_translation::translate("extended_hello_world");

	return 0;
}
