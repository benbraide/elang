#include "../asm/instruction_operand/register_instruction_operand.h"
#include "../asm/instruction/extended/extended_mov_instruction.h"

#include "machine.h"
#include "register_store.h"

elang::vm::syscall_register_store::syscall_register_store(){
	list_.reserve(13);
	float_list_.reserve(8);

	list_.push_back("rax");
	list_.push_back("rbx");
	list_.push_back("rcx");
	list_.push_back("rdx");
	list_.push_back("rsi");
	list_.push_back("rdi");

	list_.push_back("r9");
	list_.push_back("r10");
	list_.push_back("r11");
	list_.push_back("r12");
	list_.push_back("r13");
	list_.push_back("r14");
	list_.push_back("r15");

	float_list_.push_back("xmm0");
	float_list_.push_back("xmm1");
	float_list_.push_back("xmm2");
	float_list_.push_back("xmm3");
	float_list_.push_back("xmm4");
	float_list_.push_back("xmm5");
	float_list_.push_back("xmm6");
	float_list_.push_back("xmm7");
}

elang::vm::machine_register *elang::vm::syscall_register_store::get(unsigned int index, bool float_){
	auto &list = (float_ ? float_list_ : list_);
	return ((list.size() <= index) ? nullptr : machine::register_manager.find(list[index]));
}

elang::vm::machine_register *elang::vm::register_store::get(value_type_id_type value_type){
	init_();
	if (value_type == value_type_id_type::unknown)
		return nullptr;

	if (value_type == value_type_id_type::float_){
		if (available_float_list_.empty())
			return nullptr;

		auto entry = available_float_list_[0];
		available_float_list_.erase(available_float_list_.begin());
		used_list_.push_back(entry);

		return entry;
	}

	if (available_list_.empty())
		return nullptr;

	auto entry = available_list_[0];
	available_list_.erase(available_list_.begin());
	integral_used_list_.push_back(entry);
	
	switch (value_type){
	case value_type_id_type::byte:
		used_list_.push_back(entry->byte);
		return entry->byte;
	case value_type_id_type::word:
		used_list_.push_back(entry->word);
		return entry->word;
	case value_type_id_type::dword:
		used_list_.push_back(entry->dword);
		return entry->dword;
	default:
		break;
	}

	used_list_.push_back(entry->qword);
	return entry->qword;
}

elang::vm::machine_register *elang::vm::register_store::get_used(value_type_id_type value_type){
	if (used_list_.empty())
		return nullptr;

	auto entry = used_list_[0];
	used_list_.erase(used_list_.begin());

	if (entry->type_id() != value_type_id_type::float_){
		available_list_.push_back(integral_used_list_[0]);
		integral_used_list_.erase(integral_used_list_.begin());
	}
	else//Float
		available_float_list_.push_back(entry);

	if (value_type == value_type_id_type::unknown || entry->type_id() == value_type)
		return entry;//No conversion necessary

	if (value_type < entry->type_id())//Warn
		machine::compiler.add_warning(compiler_warning::type_coercion);

	auto target_reg = get_(value_type);
	auto lop = std::make_shared<elang::easm::instruction::register_operand>(*target_reg);
	auto rop = std::make_shared<elang::easm::instruction::register_operand>(*entry);

	auto instruction = std::make_shared<elang::easm::instruction::extended_mov>(std::vector<elang::easm::instruction::operand_base::ptr_type>({ lop, rop }));
	elang::vm::machine::compiler.section(elang::easm::section_id::text).add(instruction);//Add conversion instruction

	return target_reg;
}

void elang::vm::register_store::init_(){
	if (!cache_.empty())
		return;//Already initialized

	cache_.reserve(4);
	available_list_.reserve(4);
	available_float_list_.reserve(4);
	used_list_.reserve(4);

	cache_.push_back(info_type{
		machine::register_manager.find("rax"),
		machine::register_manager.find("eax"),
		machine::register_manager.find("ax"),
		machine::register_manager.find("al")
	});

	cache_.push_back(info_type{
		machine::register_manager.find("rbx"),
		machine::register_manager.find("ebx"),
		machine::register_manager.find("bx"),
		machine::register_manager.find("bl")
	});

	cache_.push_back(info_type{
		machine::register_manager.find("rcx"),
		machine::register_manager.find("ecx"),
		machine::register_manager.find("cx"),
		machine::register_manager.find("cl")
	});

	cache_.push_back(info_type{
		machine::register_manager.find("rdx"),
		machine::register_manager.find("edx"),
		machine::register_manager.find("dx"),
		machine::register_manager.find("dl")
	});

	available_list_.push_back(&cache_[0]);
	available_list_.push_back(&cache_[1]);
	available_list_.push_back(&cache_[2]);
	available_list_.push_back(&cache_[3]);

	available_float_list_.push_back(machine::register_manager.find("xmm0"));
	available_float_list_.push_back(machine::register_manager.find("xmm1"));
	available_float_list_.push_back(machine::register_manager.find("xmm2"));
	available_float_list_.push_back(machine::register_manager.find("xmm3"));
}

elang::vm::machine_register *elang::vm::register_store::get_(value_type_id_type value_type){
	if (value_type == value_type_id_type::float_)
		return available_float_list_[0];

	auto entry = available_list_[0];
	switch (value_type){
	case value_type_id_type::byte:
		return entry->byte;
	case value_type_id_type::word:
		return entry->word;
	case value_type_id_type::dword:
		return entry->dword;
	default:
		break;
	}

	return entry->qword;
}
