#pragma once

#ifndef ELANG_COMMON_UTILS_H
#define ELANG_COMMON_UTILS_H

#include <string>

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
			for (; iter != end; ++iter)
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
	};
}

#endif /* !ELANG_COMMON_UTILS_H */
