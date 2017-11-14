#include "grammar/grammar_source.h"
#include "grammar/parser/asm_parser.h"
#include "grammar/parser/expression_parser.h"

int main(){
	namespace x3 = boost::spirit::x3;
	namespace ast = elang::grammar::ast;
	namespace parser = elang::grammar::parser;
	namespace vm = elang::vm;
	namespace common = elang::common;

	elang::grammar::file_source source("basic", elang::grammar::file_source_options{ "test/asm", "easm" });
	ast::asm_instruction_set iset;

	auto r = boost::spirit::x3::phrase_parse(source.begin(), source.end(), parser::asm_instruction_set, parser::asm_skip, iset);
	ast::asm_traverser::translate(iset);

	vm::machine::asm_translation.bundle();
	vm::machine::asm_translation.print(vm::machine::out_writer);

	vm::machine::run();
	vm::machine::shutdown(false);

	return 0;
}
