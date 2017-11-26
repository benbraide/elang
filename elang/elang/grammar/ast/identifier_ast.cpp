#include "identifier_ast.h"

elang::vm::symbol_entry *elang::grammar::ast::identifier_resolver_helper::resolve(const operator_identifier &ast, elang::vm::storage_symbol_entry *search_context){
	return boost::apply_visitor(operator_identifier_resolver(search_context), ast.value);
}
