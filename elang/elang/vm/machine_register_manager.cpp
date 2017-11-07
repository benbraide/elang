#include "machine.h"

elang::vm::machine_register_manager::machine_register_manager(){
	add_("rax", "r0", "eax", "ax", "al", "ah");
	add_("rbx", "r1", "ebx", "bx", "bl", "bh");
	add_("rcx", "r2", "ecx", "cx", "cl", "ch");
	add_("rdx", "r3", "edx", "dx", "dl", "dh");

	add_("rsp", "r4", "esp", "sp", "", "");
	add_("rbp", "r5", "ebp", "bp", "", "");
	add_("rsi", "r6", "esi", "si", "", "");
	add_("rdi", "r7", "edi", "di", "", "");
	add_("rip", "r8", "eip", "ip", "", "");

	add_qword_("r", 9, 15);
	add_qword_("$cmpt", -1, -1);

	add_float_<long double>("xmm", 0, 7);

	machine::cached_registers.instruction_pointer = find("rip");
	machine::cached_registers.stack_pointer = find("rsp");
	machine::cached_registers.base_pointer = find("rbp");
	machine::cached_registers.compare_target = find("$cmpt");
}

elang::vm::machine_register *elang::vm::machine_register_manager::find(std::string key) const{
	utils::to_lower(key);

	auto entry = map_.find(key);
	if (entry == map_.end())//Error
		throw error_type::not_found;

	return entry->second.get();
}

const elang::vm::machine_register_manager::map_type &elang::vm::machine_register_manager::content() const{
	return map_;
}

void elang::vm::machine_register_manager::add_(const std::string &name, const std::string &alias, const std::string &_32, const std::string &_16, const std::string &low, const std::string &high){
	auto value = std::make_shared<basic_machine_register<__int64>>(name);

	auto _32_value = value->low(_32);
	auto _16_value = _32_value->low(_16);
	auto _8_value = _16_value->low(low);

	if (!low.empty())
		map_[low] = _8_value;

	if (!high.empty())
		map_[high] = _16_value->high(high);

	map_[_16] = _16_value;
	map_[_32] = _32_value;

	map_[name] = value;
	if (!alias.empty()){
		map_[alias + "b"] = _8_value->alias(alias + "b");
		map_[alias + "w"] = _16_value->alias(alias + "w");
		map_[alias + "d"] = _32_value->alias(alias + "d");
		map_[alias] = value->alias(alias);
	}
}

void elang::vm::machine_register_manager::add_qword_(const std::string &prefix, int from, int to){
	std::string name;

	register_ptr_type value;
	register_ptr_type dword_value;
	register_ptr_type word_value;
	register_ptr_type byte_value;

	for (; from <= to; ++from){//Add entries
		if (from < 0)
			name = prefix;
		else//Append index
			name = (prefix + std::to_string(from));

		value = std::make_shared<basic_machine_register<__int64>>(name);
		dword_value = value->low(name + "d");
		word_value = dword_value->low(name + "w");
		byte_value = word_value->low(name + "b");

		map_[name + "b"] = byte_value;
		map_[name + "w"] = word_value;
		map_[name + "d"] = dword_value;
		map_[name] = value;
	}
}
