#pragma once

#ifndef ELANG_OUTPUT_WRITER_H
#define ELANG_OUTPUT_WRITER_H

#include <mutex>
#include <string>
#include <memory>

#include "macro.h"

namespace elang::common{
	enum class output_writer_manip : unsigned int{
		nil					= (0 << 0x0000),
		flush				= (1 << 0x0000),
		newline				= (1 << 0x0001),
	};

	class output_writer{
	public:
		typedef output_writer_manip manip_type;
		typedef std::size_t size_type;

		enum class state_type : unsigned int{
			nil					= (0 << 0x0000),
			narrow_written		= (1 << 0x0000),
			wide_written		= (1 << 0x0001),
		};

		virtual ~output_writer() = default;

		virtual output_writer &begin() = 0;

		virtual output_writer &end() = 0;

		virtual output_writer &write(manip_type manip, bool wide = false) = 0;

		virtual output_writer &write_char(char value) = 0;

		virtual output_writer &write_char(wchar_t value) = 0;

		virtual output_writer &write(__int8 value) = 0;

		virtual output_writer &write(unsigned __int8 value) = 0;

		virtual output_writer &write(__int16 value) = 0;

		virtual output_writer &write(unsigned __int16 value) = 0;

		virtual output_writer &write(__int32 value) = 0;

		virtual output_writer &write(unsigned __int32 value) = 0;

		virtual output_writer &write(__int64 value) = 0;

		virtual output_writer &write(unsigned __int64 value) = 0;

		virtual output_writer &write(float value) = 0;

		virtual output_writer &write(double value) = 0;

		virtual output_writer &write(long double value) = 0;

		virtual output_writer &write(const char *value, size_type count = 0u) = 0;

		virtual output_writer &write(const wchar_t *value, size_type count = 0u) = 0;

		virtual output_writer &write(const std::string &value) = 0;

		virtual output_writer &write(const std::wstring &value) = 0;
	};

	ELANG_MAKE_OPERATORS(output_writer_manip);
	ELANG_MAKE_OPERATORS(output_writer::state_type);
}

#endif /* !ELANG_OUTPUT_WRITER_H */
