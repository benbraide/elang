#pragma once

#ifndef ELANG_STREAM_INPUT_READER_H
#define ELANG_STREAM_INPUT_READER_H

#include <fstream>
#include <iostream>

#include "input_reader.h"

namespace elang::common{
	template <class stream_type, class wide_stream_type>
	class stream_input_reader : public input_reader{
	public:
		typedef stream_type stream_type;
		typedef wide_stream_type wide_stream_type;

		typedef std::recursive_mutex lock_type;
		typedef std::lock_guard<lock_type> guard_type;

		stream_input_reader(stream_type &stream, wide_stream_type &wide_stream)
			: stream_(stream), wide_stream_(wide_stream){}

		virtual input_reader &begin() override{
			lock_.lock();
			return *this;
		}

		virtual input_reader &end() override{
			lock_.unlock();
			return *this;
		}

		virtual input_reader &read(char &value) override{
			stream_.get(value);
			return *this;
		}

		virtual input_reader &read(wchar_t &value) override{
			wide_stream_.get(value);
			return *this;
		}

		virtual input_reader &read(char *buffer, size_type count) override{
			stream_.read(buffer, count);
			return *this;
		}

		virtual input_reader &read(wchar_t *buffer, size_type count) override{
			wide_stream_.read(buffer, count);
			return *this;
		}

	protected:
		stream_type &stream_;
		wide_stream_type &wide_stream_;
		lock_type lock_;
	};
}

#endif /* !ELANG_STREAM_INPUT_READER_H */
