#pragma once

#ifndef ELANG_STREAM_OUTPUT_WRITER_H
#define ELANG_STREAM_OUTPUT_WRITER_H

#include <fstream>
#include <iostream>

#include "output_writer.h"

namespace elang{
	namespace common{
		template <class stream_type, bool is_wide>
		class stream_output_writer : public output_writer{
		public:
			typedef stream_type stream_type;

			explicit stream_output_writer(stream_type &stream)
				: stream_(stream){}

			virtual output_writer &operator <<(manip_type manip) override{
				switch (manip){
				case manip_type::flush:
					stream_ << std::flush;
					break;
				default:
					return output_writer::operator<<(manip);
				}

				return *this;
			}

			virtual output_writer &operator <<(__int8 value) override{
				return print_(value);
			}

			virtual output_writer &operator <<(unsigned __int8 value) override{
				return print_(value);
			}

			virtual output_writer &operator <<(__int16 value) override{
				return print_(value);
			}

			virtual output_writer &operator <<(unsigned __int16 value) override{
				return print_(value);
			}

			virtual output_writer &operator <<(__int32 value) override{
				return print_(value);
			}

			virtual output_writer &operator <<(unsigned __int32 value) override{
				return print_(value);
			}

			virtual output_writer &operator <<(__int64 value) override{
				return print_(value);
			}

			virtual output_writer &operator <<(unsigned __int64 value) override{
				return print_(value);
			}

			virtual output_writer &operator <<(float value) override{
				return print_(value);
			}

			virtual output_writer &operator <<(double value) override{
				return print_(value);
			}

			virtual output_writer &operator <<(long double value) override{
				return print_(value);
			}

			virtual output_writer &operator <<(const char *value) override{
				return print_string_(value, std::bool_constant<is_wide>());
			}

			virtual output_writer &operator <<(const wchar_t *value) override{
				return print_wstring_(value, std::bool_constant<is_wide>());
			}

			virtual output_writer &operator <<(const std::string &value) override{
				return print_string_(value, std::bool_constant<is_wide>());
			}

			virtual output_writer &operator <<(const std::wstring &value) override{
				return print_wstring_(value, std::bool_constant<is_wide>());
			}

		protected:
			virtual stream_type &pre_write_and_return_stream_(){
				pre_write_();
				return stream_;
			}

			template <typename value_type>
			output_writer &print_string_(value_type value, std::true_type){
				throw error_type::cannot_write_narrow;
			}

			template <typename value_type>
			output_writer &print_string_(value_type value, std::false_type){
				return print_(value);
			}

			template <typename value_type>
			output_writer &print_wstring_(value_type value, std::true_type){
				return print_(value);
			}

			template <typename value_type>
			output_writer &print_wstring_(value_type value, std::false_type){
				throw error_type::cannot_write_wide;
			}

			template <typename value_type>
			output_writer &print_(value_type value){
				pre_write_and_return_stream_() << value;
				return *this;
			}

			stream_type &stream_;
		};

		using ostream_output_writer_type = stream_output_writer<std::ostream, false>;
		using wostream_output_writer_type = stream_output_writer<std::wostream, true>;

		ostream_output_writer_type out_writer(std::cout);
		ostream_output_writer_type err_writer(std::cerr);
		ostream_output_writer_type log_writer(std::clog);

		wostream_output_writer_type wout_writer(std::wcout);
		wostream_output_writer_type werr_writer(std::wcerr);
		wostream_output_writer_type wlog_writer(std::wclog);

		template <bool is_wide>
		struct file_stream_output_writer_handle_type{
			typedef std::ofstream type;
		};

		template <>
		struct file_stream_output_writer_handle_type<true>{
			typedef std::wofstream type;
		};

		template <bool is_wide>
		class basic_file_stream_output_writer : public stream_output_writer<typename file_stream_output_writer_handle_type<is_wide>::type, is_wide>{
		public:
			typedef typename file_stream_output_writer_handle_type<is_wide>::type handle_type;
			typedef stream_output_writer<handle_type, is_wide> base_type;

			template <typename... args_types>
			explicit basic_file_stream_output_writer(args_types &&... args)
				: base_type(handle_), handle_(std::forward<args_types>(args)...){}

		protected:
			handle_type handle_;
		};

		using file_stream_output_writer = basic_file_stream_output_writer<false>;
		using wfile_stream_output_writer = basic_file_stream_output_writer<true>;
	}
}

#endif /* !ELANG_STREAM_OUTPUT_WRITER_H */
