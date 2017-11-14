#pragma once

#ifndef ELANG_GRAMMAR_SOURCE_H
#define ELANG_GRAMMAR_SOURCE_H

#include <string>

#include <boost/filesystem.hpp>
#include <boost/iostreams/device/mapped_file.hpp>

namespace elang::grammar{
	enum class source_error{
		nil,
		file_not_found,
		already_opened,
		open_failed,
	};

	class source_base{
	public:
		virtual ~source_base() = default;

		virtual const char *begin() const = 0;

		virtual const char *end() const = 0;
	};

	struct file_source_options{
		std::string path;
		std::string extension;
	};

	class file_source : public source_base{
	public:
		typedef source_error error_type;
		typedef boost::iostreams::mapped_file mapped_file_type;

		explicit file_source(const file_source_options &options = file_source_options{});

		explicit file_source(const std::string &file, const file_source_options &options = file_source_options{});

		virtual ~file_source();

		virtual const char *begin() const override;

		virtual const char *end() const override;

		void open(std::string file);

		void close();

		bool is_open() const;

	private:
		file_source_options options_;
		mapped_file_type buffer_;
	};
}

#endif /* !ELANG_GRAMMAR_SOURCE_H */
