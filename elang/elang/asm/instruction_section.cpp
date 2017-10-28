#include "instruction_section.h"

elang::easm::instruction_section::instruction_section(id_type id)
	: id_(id){}

elang::easm::instruction_section::id_type elang::easm::instruction_section::id() const{
	return id_;
}

void elang::easm::instruction_section::create() const{

}

void elang::easm::instruction_section::print(writer_type &writer, writer_type &wide_writer) const{
	writer << "section .";
	switch (id_){
	case id_type::rodata:
		writer << "rodata" << writer_type::manip_type::flush;
		break;
	case id_type::data:
		writer << "data" << writer_type::manip_type::flush;
		break;
	case id_type::text:
		writer << "text" << writer_type::manip_type::flush;
		break;
	default:
		break;
	}
}
