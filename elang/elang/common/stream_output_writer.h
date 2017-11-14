#pragma once

#ifndef ELANG_STREAM_OUTPUT_WRITER_H
#define ELANG_STREAM_OUTPUT_WRITER_H

#include <fstream>
#include <iostream>

#include "output_writer.h"

namespace elang::common{
	template <class stream_type, class wide_stream_type>
	class stream_output_writer : public output_writer{
	public:
		typedef stream_type stream_type;
		typedef wide_stream_type wide_stream_type;

		typedef std::recursive_mutex lock_type;
		typedef std::lock_guard<lock_type> guard_type;

		stream_output_writer(stream_type &stream, wide_stream_type &wide_stream)
			: stream_(stream), wide_stream_(wide_stream), states_(state_type::nil){}

		virtual output_writer &begin() override{
			lock_.lock();
			return *this;
		}

		virtual output_writer &end() override{
			write_(stream_, manip_type::flush, false);
			write_(wide_stream_, manip_type::flush, true);
			lock_.unlock();
			return *this;
		}

		virtual output_writer &write(manip_type manip, bool wide = false) override{
			for (auto flag = manip_type::flush; flag <= manip_type::newline; flag = static_cast<manip_type>(static_cast<unsigned int>(flag) + 1)){
				if (!ELANG_IS(manip, flag))
					continue;//Flag not specified

				if (wide)
					write_(wide_stream_, flag, wide);
				else//Narrow
					write_(stream_, flag, wide);
			}

			return *this;
		}

		virtual output_writer &write_char(char value) override{
			return write_(value);
		}

		virtual output_writer &write_char(wchar_t value) override{
			pre_write_(true);
			wide_stream_ << value;
			return *this;
		}

		virtual output_writer &write(__int8 value) override{
			return write_(value);
		}

		virtual output_writer &write(unsigned __int8 value) override{
			return write_(value);
		}

		virtual output_writer &write(__int16 value) override{
			return write_(value);
		}

		virtual output_writer &write(unsigned __int16 value) override{
			return write_(value);
		}

		virtual output_writer &write(__int32 value) override{
			return write_(value);
		}

		virtual output_writer &write(unsigned __int32 value) override{
			return write_(value);
		}

		virtual output_writer &write(__int64 value) override{
			return write_(value);
		}

		virtual output_writer &write(unsigned __int64 value) override{
			return write_(value);
		}

		virtual output_writer &write(float value) override{
			return write_(value);
		}

		virtual output_writer &write(double value) override{
			return write_(value);
		}

		virtual output_writer &write(long double value) override{
			return write_(value);
		}

		virtual output_writer &write(const char *value, size_type count = 0u) override{
			pre_write_(false);
			if (count == 0u)
				stream_ << value;
			else//Write 'count' chars
				stream_.write(value, count);
			return *this;
		}

		virtual output_writer &write(const wchar_t *value, size_type count = 0u) override{
			pre_write_(true);
			if (count == 0u)
				wide_stream_ << value;
			else//Write 'count' chars
				wide_stream_.write(value, count);
			return *this;
		}

		virtual output_writer &write(const std::string &value) override{
			pre_write_(false);
			stream_ << value;
			return *this;
		}

		virtual output_writer &write(const std::wstring &value) override{
			pre_write_(true);
			wide_stream_ << value;
			return *this;
		}

	protected:
		void pre_write_(bool wide){
			if (wide){
				if (ELANG_IS(states_, state_type::narrow_written)){
					write(manip_type::flush, false);
					ELANG_REMOVE(states_, state_type::narrow_written);
				}

				ELANG_SET(states_, state_type::wide_written);
			}
			else{//Writing narrow
				if (ELANG_IS(states_, state_type::wide_written)){
					write(manip_type::flush, true);
					ELANG_REMOVE(states_, state_type::wide_written);
				}

				ELANG_SET(states_, state_type::narrow_written);
			}
		}

		template <typename stream_type>
		void write_(stream_type &stream, manip_type flag, bool wide){
			switch (flag){
			case manip_type::flush:
				stream_ << std::flush;
				if (wide)
					ELANG_REMOVE(states_, state_type::wide_written);
				else//Narrow
					ELANG_REMOVE(states_, state_type::narrow_written);
				break;
			case manip_type::newline:
				stream_ << std::endl;
				if (wide)
					ELANG_REMOVE(states_, state_type::wide_written);
				else//Narrow
					ELANG_REMOVE(states_, state_type::narrow_written);
				break;
			default:
				break;
			}
		}

		template <typename value_type>
		output_writer &write_(value_type value){
			pre_write_(false);
			stream_ << value;
			return *this;
		}

		stream_type &stream_;
		wide_stream_type &wide_stream_;
		state_type states_;
		lock_type lock_;
	};
}

#endif /* !ELANG_STREAM_OUTPUT_WRITER_H */
