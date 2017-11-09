#pragma once

#ifndef ELANG_GRAMMAR_UTILS_H
#define ELANG_GRAMMAR_UTILS_H

#include "../common/common_utils.h"

#include "ast/ast.h"

namespace elang::grammar{
	namespace x3 = boost::spirit::x3;

	template <typename value_type>
	struct real_policies : x3::real_policies<value_type>{
		static bool const expect_dot = true;
		static bool const allow_leading_dot = true;
		static bool const allow_trailing_dot = true;
	};

	using float_parser = x3::real_parser<float, real_policies<float>>;
	using double_parser = x3::real_parser<double, real_policies<double>>;
	using long_double_parser = x3::real_parser<long double, real_policies<long double>>;

	float_parser const float_ = {};
	double_parser const double_ = {};
	long_double_parser const long_double_ = {};

	float_parser const f32_ = {};
	double_parser const f64_ = {};
	long_double_parser const f128_ = {};

	struct elang_identifier_ast{
		char first;
		std::vector<char> rest;
	};

	class utils{
	public:
		template <typename parser_type>
		static auto keyword(parser_type &&parser){
			return x3::lexeme[parser >> !x3::char_("$_A-Za-z0-9")];
		};

		static auto keyword(const char *value){
			return keyword(x3::lit(value));
		}

		static void identifier_to_string(const elang_identifier_ast &ast, std::string &value){
			value.reserve(ast.rest.size() + 2);
			value.append(1, ast.first);
			value.append(ast.rest.data(), ast.rest.size());
		}
	};

	x3::rule<class elang_identifier, elang_identifier_ast> const elang_identifier = "elang_identifier";
	auto const elang_identifier_def = x3::lexeme[x3::char_("$_A-Za-z") >> *x3::char_("$_A-Za-z0-9")];

	template <typename target_type>
	using hex_parser = x3::uint_parser<target_type, 16>;

	template <typename target_type>
	using oct_parser = x3::uint_parser<target_type, 8>;

	template <typename target_type>
	using bin_parser = x3::uint_parser<target_type, 2>;

	BOOST_SPIRIT_DEFINE(elang_identifier);
}

BOOST_FUSION_ADAPT_STRUCT(
	elang::grammar::elang_identifier_ast,
	(char, first)
	(std::vector<char>, rest)
)

#endif /* !ELANG_GRAMMAR_UTILS_H */
