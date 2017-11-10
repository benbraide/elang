#include "identifier_ast.h"

elang::vm::symbol_entry *elang::grammar::ast::identifier_resolver_helper::resolve(const operator_identifier &ast){
	return boost::apply_visitor(operator_identifier_resolver(), ast.value);
}
