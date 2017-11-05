#pragma once

#ifndef ELANG_GRAMMAR_UTILS_H
#define ELANG_GRAMMAR_UTILS_H

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

		static void escape_string(const char *data, const char *end, std::string out){
			out.reserve(end - data + 1);//Reserve maximum possible size
			for (; data != end; ++data)
				out.append(1u, escaped_char<char>(data, end));
		}

		static void escape_string(const char *data, const char *end, std::wstring out){
			out.reserve(end - data + 1);//Reserve maximum possible size
			for (; data != end; ++data)
				out.append(1u, escaped_char<wchar_t>(data, end));
		}

		template <typename char_type>
		static char_type escaped_char(const char *&data, const char *end){
			auto c = *(data++);
			switch (c){
			case 'a':
				return static_cast<char_type>('\a');
			case 'b':
				return static_cast<char_type>('\b');
			case 'f':
				return static_cast<char_type>('\f');
			case 'n':
				return static_cast<char_type>('\n');
			case 'r':
				return static_cast<char_type>('\r');
			case 't':
				return static_cast<char_type>('\t');
			case 'v':
				return static_cast<char_type>('\v');
			case '"':
				return static_cast<char_type>('\"');
			case '\'':
				return static_cast<char_type>('\'');
			case '\\':
				return static_cast<char_type>('\\');
			case '0':
				return escaped_oct_char<char_type>(data, end, std::bool_constant<std::is_same<char_type, char>::value>());
			case 'x':
				return escaped_hex_char<char_type>(data, end, std::bool_constant<std::is_same<char_type, char>::value>());
			default:
				break;
			}

			return static_cast<char_type>(c);
		}

		template <typename char_type>
		static char_type escaped_oct_char(const char *&data, const char *end, std::true_type){//Narrow
			if (data == end || !isdigit(*data))
				return static_cast<char_type>('\0');

			auto start = data++;//Advance past digit
			if (data != end && isdigit(*data))
				++data;//Double digits

			return static_cast<char_type>(std::stoi(std::string(start, data), nullptr, 8));
		}

		template <typename char_type>
		static char_type escaped_oct_char(const char *&data, const char *end, std::false_type){//Wide
			if (data == end || !isdigit(*data))
				return static_cast<char_type>('\0');

			std::wstring digits;
			digits.reserve(5);
			digits.append(1u, static_cast<char_type>(*(data++)));

			for (auto i = 0; i < 3 && data != end && isdigit(*data); ++i)
				digits.append(1u, static_cast<char_type>(*(data++)));

			return static_cast<char_type>(std::stoi(digits), nullptr, 8);
		}

		template <typename char_type>
		static char_type escaped_hex_char(const char *&data, const char *end, std::true_type){//Narrow
			if (data == end || !isxdigit(*data))
				return static_cast<char_type>('\0');

			auto start = data++;//Advance past digit
			if (data != end && isxdigit(*data))
				++data;//Double digits

			return static_cast<char_type>(std::stoi(std::string(start, data), nullptr, 16));
		}

		template <typename char_type>
		static char_type escaped_hex_char(const char *&data, const char *end, std::false_type){//Wide
			if (data == end || !isxdigit(*data))
				return static_cast<char_type>('\0');

			std::wstring digits;
			digits.reserve(5);
			digits.append(1u, static_cast<char_type>(*(data++)));

			for (auto i = 0; i < 3 && data != end && isxdigit(*data); ++i)
				digits.append(1u, static_cast<char_type>(*(data++)));

			return static_cast<char_type>(std::stoi(digits), nullptr, 16);
		}
	};

	x3::rule<class elang_identifier, elang_identifier_ast> const elang_identifier = "elang_identifier";
	auto const elang_identifier_def = x3::lexeme[x3::char_("$_A-Za-z") >> *x3::char_("$_A-Za-z0-9")];

	template <typename target_type>
	using signed_hex_parser = x3::uint_parser<target_type, 16>;

	template <typename target_type>
	using signed_oct_parser = x3::uint_parser<target_type, 8>;

	template <typename target_type>
	using signed_bin_parser = x3::uint_parser<target_type, 2>;

	BOOST_SPIRIT_DEFINE(elang_identifier);
}

BOOST_FUSION_ADAPT_STRUCT(
	elang::grammar::elang_identifier_ast,
	(char, first)
	(std::vector<char>, rest)
)

#endif /* !ELANG_GRAMMAR_UTILS_H */
