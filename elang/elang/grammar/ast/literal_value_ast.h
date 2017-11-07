#pragma once

#ifndef ELANG_LITERAL_VALUE_AST_H
#define ELANG_LITERAL_VALUE_AST_H

#include "../../common/numeric_literal_suffix.h"
#include "../../common/string_quote_type.h"

#include "asm_ast.h"

#define ELANG_AST_NUMERIC_SUFFIX boost::optional<elang::common::numeric_literal_suffix>

ELANG_AST_BEGIN

ELANG_AST_DECLARE_PAIR(integral_literal, __int64, ELANG_AST_NUMERIC_SUFFIX)
ELANG_AST_DECLARE_PAIR(real_literal, long double, ELANG_AST_NUMERIC_SUFFIX)
ELANG_AST_DECLARE_SINGLE_VARIANT(numeric_literal, integral_literal, real_literal)

ELANG_AST_DECLARE_PAIR(string_literal, elang::common::string_quote_type, std::vector<char>)
ELANG_AST_DECLARE_SINGLE_VARIANT(literal_value, numeric_literal, string_literal)

ELANG_AST_END

ELANG_AST_ADAPT_PAIR(integral_literal)
ELANG_AST_ADAPT_PAIR(real_literal)
ELANG_AST_ADAPT_SINGLE(numeric_literal)

ELANG_AST_ADAPT_PAIR(string_literal)
ELANG_AST_ADAPT_SINGLE(literal_value)

#endif /* !ELANG_LITERAL_VALUE_AST_H */
