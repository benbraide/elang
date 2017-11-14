#pragma once

#ifndef ELANG_INPUT_READER_H
#define ELANG_INPUT_READER_H

#include <mutex>
#include <string>
#include <memory>

namespace elang::common{
	class input_reader{
	public:
		typedef std::size_t size_type;

		virtual ~input_reader() = default;

		virtual input_reader &begin() = 0;

		virtual input_reader &end() = 0;

		virtual input_reader &read(char &value) = 0;

		virtual input_reader &read(wchar_t &value) = 0;

		virtual input_reader &read(char *buffer, size_type count) = 0;

		virtual input_reader &read(wchar_t *buffer, size_type count) = 0;
	};
}

#endif /* !ELANG_INPUT_READER_H */
