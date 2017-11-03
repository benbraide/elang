#include "grammar_source.h"

elang::grammar::file_source::file_source(const file_source_options &options)
	: options_(options){}

elang::grammar::file_source::file_source(const std::string &file, const file_source_options &options)
	: options_(options){
	open(file);
}

void elang::grammar::file_source::open(std::string file){
	if (buffer_.is_open())
		throw error_type::already_opened;
	
	boost::filesystem::path path(file);
	if (!options_.path.empty() && path.is_relative())//Use base path
		path = (boost::filesystem::absolute(options_.path) / path);

	if (!boost::filesystem::exists(path) && !options_.extension.empty() && !path.has_extension()){//Append extension
		if (!boost::filesystem::exists(path = (path.string() + "." + options_.extension)))
			throw error_type::file_not_found;
	}
	else//File not found
		throw error_type::file_not_found;

	buffer_.open(path, mapped_file_type::readonly);
	if (!buffer_.is_open())//Failed to open file
		throw error_type::open_failed;
}

void elang::grammar::file_source::close(){
	buffer_.close();
}

bool elang::grammar::file_source::is_open() const{
	return buffer_.is_open();
}

const char *elang::grammar::file_source::begin() const{
	return buffer_.const_begin();
}

const char *elang::grammar::file_source::end() const{
	return buffer_.const_end();
}
