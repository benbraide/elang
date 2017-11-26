#pragma once

#ifndef ELANG_TYPE_PARSER_H
#define ELANG_TYPE_PARSER_H

#include "../ast/type_ast.h"

#include "identifier_parser.h"

namespace elang::grammar::parser{
	namespace x3 = boost::spirit::x3;

	struct primitive_type_symbols : x3::symbols<elang::common::primitive_type_id>{
		primitive_type_symbols(){
			add
				("void", elang::common::primitive_type_id::void_)
				("auto", elang::common::primitive_type_id::auto_)
				("bool", elang::common::primitive_type_id::bool_)
				("char", elang::common::primitive_type_id::char_)
				("wchar", elang::common::primitive_type_id::wchar_)
				("int8", elang::common::primitive_type_id::int8_)
				("uint8", elang::common::primitive_type_id::uint8_)
				("int16", elang::common::primitive_type_id::int16_)
				("uint16", elang::common::primitive_type_id::uint16_)
				("int32", elang::common::primitive_type_id::int32_)
				("uint32", elang::common::primitive_type_id::uint32_)
				("int64", elang::common::primitive_type_id::int64_)
				("uint64", elang::common::primitive_type_id::uint64_)
				("int", elang::common::primitive_type_id::int32_)
				("uint", elang::common::primitive_type_id::uint32_)
				("float", elang::common::primitive_type_id::float_)
				;
		}
	} primitive_type_symbols_;

	auto resolve_type = [](auto &ctx){
		ast::storage_resolver resolver(true);//Search for storages only
		auto entry = resolver(x3::_attr(ctx));

		x3::_val(ctx) = entry;
		x3::_pass(ctx) = (entry != nullptr && entry->id() == elang::vm::symbol_entry_id::type);
	};

	x3::rule<class primitive_type, ast::primitive_type> const primitive_type = "primitive_type";
	x3::rule<class user_type, ast::user_type> const user_type = "user_type";

	x3::rule<class const_storage_type, ast::const_storage_type> const const_storage_type = "const_storage_type";

	x3::rule<class pointer_type, ast::pointer_type> const pointer_type = "pointer_type";
	x3::rule<class array_type, ast::array_type> const array_type = "array_type";

	x3::rule<class ref_type, ast::ref_type> const ref_type = "ref_type";
	x3::rule<class vref_type, ast::vref_type> const vref_type = "vref_type";

	x3::rule<class const_target_type, ast::const_target_type> const const_target_type = "const_target_type";

	x3::rule<class type, ast::type> const type = "type";
	x3::rule<class variadic_type, ast::variadic_type> const variadic_type = "variadic_type";

	auto const primitive_type_def = utils::keyword(primitive_type_symbols_);
	auto const user_type_def = non_operator_identifier_compatible[resolve_type];

	auto const const_storage_type_def = ((primitive_type | user_type) >> utils::keyword("const"));

	auto const pointer_type_def = ((const_storage_type | primitive_type | user_type) >> +x3::char_("*"));
	auto const array_type_def = ((pointer_type | const_storage_type | primitive_type | user_type) >> x3::lit("[") > ']');

	auto const ref_type_def = ((pointer_type | const_storage_type | primitive_type | user_type) >> "&");
	auto const vref_type_def = ((pointer_type | const_storage_type | primitive_type | user_type) >> "&&");

	auto const const_target_type_def = (utils::keyword("const") >> (vref_type | ref_type | array_type | pointer_type | const_storage_type | primitive_type | user_type));

	auto const type_def = (const_target_type | vref_type | ref_type | array_type | pointer_type | const_storage_type | primitive_type | user_type);
	auto const variadic_type_def = (type >> "...");

	BOOST_SPIRIT_DEFINE(
		primitive_type,
		user_type,
		const_storage_type,
		pointer_type,
		array_type,
		ref_type,
		vref_type,
		const_target_type,
		type,
		variadic_type
	)
}

#endif /* !ELANG_TYPE_PARSER_H */
