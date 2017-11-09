#pragma once

#ifndef ELANG_IDENTIFIER_AST_H
#define ELANG_IDENTIFIER_AST_H

#include "../../vm/machine.h"

#include "asm_ast.h"
#include "operator_symbol_ast.h"

ELANG_AST_BEGIN

using identifier = elang::grammar::elang_identifier_ast;

ELANG_AST_DECLARE_SINGLE(operator_identifier, identifier)

ELANG_AST_DECLARE_SINGLE(global_qualified_identifier, identifier)

using qualified_identifier_variant = boost::variant<identifier, global_qualified_identifier>;

ELANG_AST_DECLARE_PAIR(qualified_identifier, qualified_identifier_variant, std::vector<identifier>)

struct identifier_traverser{

};

ELANG_AST_END

ELANG_AST_ADAPT_SINGLE(global_qualified_identifier)
ELANG_AST_ADAPT_PAIR(qualified_identifier)

#endif /* !ELANG_IDENTIFIER_AST_H */
