#pragma once

#ifndef ELANG_COMMON_UTILS_H
#define ELANG_COMMON_UTILS_H

#include <string>

#include "operator_id.h"

namespace elang::common{
	class utils{
	public:
		template <typename iterator_type>
		static void disable_string_escape(iterator_type iter, iterator_type end, std::string &out){
			for (; iter != end; ++iter){
				if (*iter == '\\')//Disable escape
					out.append("\\\\");
				else//Append single
					out.append(1, *iter);
			}
		}

		static void disable_string_escape(std::string &in_out){
			for (auto i = in_out.size(); i > 0u; --i){
				if (in_out[i - 1] == '\\'){//Disable escape
					if (i == in_out.size())
						in_out.append(1, '\\');
					else//Insert
						in_out.insert(i, 1, '\\');
				}
			}
		}

		template <typename iterator_type>
		static void escape_string(iterator_type iter, iterator_type end, std::string &out){
			out.reserve((end - iter) + 1);//Reserve maximum possible size
			for (; iter != end;)
				out.append(1u, escaped_char<char>(iter, end));
		}

		template <typename iterator_type>
		static void escape_string(iterator_type iter, iterator_type end, std::wstring &out){
			out.reserve((end - iter) + 1);//Reserve maximum possible size
			for (; iter != end; ++iter)
				out.append(1u, escaped_char<wchar_t>(iter, end));
		}

		template <typename char_type, typename iterator_type>
		static char_type escaped_char(iterator_type &iter, iterator_type end){
			auto c = *(iter++);
			if (c != static_cast<char_type>('\\') || iter == end)
				return c;

			switch (*(iter++)){
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
				return escaped_oct_char<char_type>(iter, end, std::bool_constant<std::is_same<char_type, char>::value>());
			case 'x':
				return escaped_hex_char<char_type>(iter, end, std::bool_constant<std::is_same<char_type, char>::value>());
			default:
				break;
			}

			return static_cast<char_type>(c);
		}

		template <typename char_type, typename iterator_type>
		static char_type escaped_oct_char(iterator_type &iter, iterator_type end, std::true_type){//Narrow
			if (iter == end || !isdigit(*iter))
				return static_cast<char_type>('\0');

			auto start = iter++;//Advance past digit
			if (iter != end && (*iter >= '0' && *iter <= '7'))
				++iter;//Double digits

			return static_cast<char_type>(std::stoi(std::string(start, iter), nullptr, 8));
		}

		template <typename char_type, typename iterator_type>
		static char_type escaped_oct_char(iterator_type &iter, iterator_type end, std::false_type){//Wide
			if (iter == end || !isdigit(*iter))
				return static_cast<char_type>('\0');

			std::wstring digits;
			digits.reserve(5);
			digits.append(1u, static_cast<char_type>(*(iter++)));

			for (auto i = 0; i < 3 && iter != end && (*iter >= static_cast<char_type>('0') && *iter <= static_cast<char_type>('7')); ++i)
				digits.append(1u, static_cast<char_type>(*(iter++)));

			return static_cast<char_type>(std::stoi(digits), nullptr, 8);
		}

		template <typename char_type, typename iterator_type>
		static char_type escaped_hex_char(iterator_type &iter, iterator_type end, std::true_type){//Narrow
			if (iter == end || !isxdigit(*iter))
				return static_cast<char_type>('\0');

			auto start = iter++;//Advance past digit
			if (iter != end && isxdigit(*iter))
				++iter;//Double digits

			return static_cast<char_type>(std::stoi(std::string(start, iter), nullptr, 16));
		}

		template <typename char_type, typename iterator_type>
		static char_type escaped_hex_char(iterator_type &iter, iterator_type end, std::false_type){//Wide
			if (iter == end || !isxdigit(*iter))
				return static_cast<char_type>('\0');

			std::wstring digits;
			digits.reserve(5);
			digits.append(1u, static_cast<char_type>(*(iter++)));

			for (auto i = 0; i < 3 && iter != end && iswxdigit(*iter); ++i)
				digits.append(1u, static_cast<char_type>(*(iter++)));

			return static_cast<char_type>(std::stoi(digits), nullptr, 16);
		}

		static std::string mangle_operator_symbol(operator_id id, const std::string &value){
			return ((id == operator_id::unknown) ? mangle_operator_symbol(id) : mangle_operator_symbol(value));
		}

		static std::string mangle_operator_symbol(operator_id id){
			switch (id){
			case operator_id::comma:
				return "c";
			case operator_id::plus:
				return "p";
			case operator_id::minus:
				return "m";
			case operator_id::times:
				return "t";
			case operator_id::divide:
				return "d";
			case operator_id::modulus:
				return "u";
			case operator_id::bitwise_or:
				return "o";
			case operator_id::bitwise_and:
				return "a";
			case operator_id::bitwise_xor:
				return "x";
			case operator_id::left_shift:
				return "ll";
			case operator_id::right_shift:
				return "gg";
			case operator_id::bitwise_inverse:
				return "i";
			case operator_id::less:
				return "l";
			case operator_id::less_or_equal:
				return "le";
			case operator_id::greater:
				return "g";
			case operator_id::greater_or_equal:
				return "ge";
			case operator_id::equality:
				return "ee";
			case operator_id::inverse_equality:
				return "ne";
			case operator_id::relational_or:
				return "oo";
			case operator_id::relational_and:
				return "aa";
			case operator_id::relational_not:
				return "n";
			case operator_id::member_access:
				return "b";
			case operator_id::member_pointer_access:
				return "mg";
			case operator_id::call:
				return "pc";
			case operator_id::index:
				return "sc";
			case operator_id::new_:
				return "w";
			case operator_id::delete_:
				return "nw";
			case operator_id::sizeof_:
				return "s";
			case operator_id::increment:
				return "pp";
			case operator_id::decrement:
				return "mm";
			case operator_id::ternary:
				return "q";
			case operator_id::assignment:
				return "e";
			case operator_id::compound_plus:
				return "pe";
			case operator_id::compound_minus:
				return "me";
			case operator_id::compound_times:
				return "te";
			case operator_id::compound_divide:
				return "de";
			case operator_id::compound_modulus:
				return "ue";
			case operator_id::compound_bitwise_or:
				return "oe";
			case operator_id::compound_bitwise_and:
				return "ae";
			case operator_id::compound_bitwise_xor:
				return "xe";
			case operator_id::compound_left_shift:
				return "lle";
			case operator_id::compound_right_shift:
				return "gge";
			default:
				break;
			}

			return "";
		}

		static std::string mangle_operator_symbol(const std::string &value){
			std::string mangled_value;

			mangled_value.reserve(value.size());
			for (auto c : value)
				mangled_value.append(1, mangle_operator_symbol_char(c));

			return mangled_value;
		}

		static char mangle_operator_symbol_char(char value){
			switch (value){
			case ',':
				return 'c';
			case '+':
				return 'p';
			case '-':
				return 'm';
			case '*':
				return 't';
			case '/':
				return 'd';
			case '%':
				return 'u';
			case '|':
				return 'o';
			case '&':
				return 'a';
			case '^':
				return 'x';
			case '~':
				return 'i';
			case '<':
				return 'l';
			case '>':
				return 'g';
			case '!':
				return 'n';
			case '.':
				return 'b';
			case '=':
				return 'e';
			default:
				break;
			}

			return '\0';
		}
	};
}

#endif /* !ELANG_COMMON_UTILS_H */
