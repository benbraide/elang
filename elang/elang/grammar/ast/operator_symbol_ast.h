#pragma once

#ifndef ELANG_OPERATOR_SYMBOL_AST_H
#define ELANG_OPERATOR_SYMBOL_AST_H

#include "../../common/operator_id.h"

#include "ast.h"

ELANG_AST_BEGIN

ELANG_AST_DECLARE_PAIR(operator_symbol, elang::common::operator_id, std::vector<char>)

ELANG_AST_END

ELANG_AST_ADAPT_PAIR(operator_symbol)

#endif /* !ELANG_OPERATOR_SYMBOL_AST_H */
