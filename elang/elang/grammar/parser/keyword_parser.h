#pragma once

#ifndef ELANG_KEYWORD_PARSER_H
#define ELANG_KEYWORD_PARSER_H

#include "../grammar_utils.h"

namespace elang::grammar::parser{
	namespace x3 = boost::spirit::x3;

	struct keyword_symbols : x3::symbols<>{
		keyword_symbols(){
			add
				("module")
				("export")
				("import")
				("void")
				("auto")
				("bool")
				("char")
				("wchar")
				("int8")
				("uint8")
				("int16")
				("uint16")
				("int32")
				("uint32")
				("int64")
				("uint64")
				("int128")
				("uint128")
				("int")
				("uint")
				("float")
				("enum")
				("union")
				("struct")
				("class")
				("namespace")
				("nullptr_t")
				("nan_t")
				("decltype")
				("static_cast")
				("reinterpret_cast")
				("static")
				("thread_local")
				("true")
				("false")
				("#ind")
				("#inf")
				("nullptr")
				("NaN")
				("using")
				("typedef")
				("operator")
				("sizeof")
				("typeid")
				("if")
				("else")
				("unless")
				("while")
				("do")
				("until")
				("for")
				("switch")
				("case")
				("default")
				("break")
				("continue")
				("return")
				("throw")
				("try")
				("catch")
				("finally")
				("new")
				("delete")
				("self")
				;
		}
	} keyword_symbols_;

	x3::rule<class keyword> const keyword = "keyword";

	auto const keyword_def = utils::keyword(keyword_symbols_);

	BOOST_SPIRIT_DEFINE(keyword)
}

#endif /* !ELANG_KEYWORD_PARSER_H */
