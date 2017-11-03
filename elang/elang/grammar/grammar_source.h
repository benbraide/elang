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

	struct file_source_options{
		std::string path;
		std::string extension;
	};

	class file_source{
	public:
		typedef source_error error_type;
		typedef boost::iostreams::mapped_file mapped_file_type;

		explicit file_source(const file_source_options &options = file_source_options{});

		explicit file_source(const std::string &file, const file_source_options &options = file_source_options{});

		void open(std::string file);

		void close();

		bool is_open() const;

		const char *begin() const;

		const char *end() const;

	private:
		file_source_options options_;
		mapped_file_type buffer_;
	};
}

#endif /* !ELANG_GRAMMAR_SOURCE_H */
