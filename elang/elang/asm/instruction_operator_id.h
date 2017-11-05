#pragma once

#ifndef ELANG_INSTRUCTION_OPERATOR_ID_H
#define ELANG_INSTRUCTION_OPERATOR_ID_H

#include <string>

namespace elang::easm{
	enum class instruction_operator_id{
		add,
		sub,
		mult,
		div,
		mod,
		and_,
		xor_,
		or_,
		sal,
		sar,
		not,
		inc,
		dec,
	};

	struct instruction_operator_utils{
		std::string static instruction_operator_id_to_string(instruction_operator_id id){
			switch (id){
			case instruction_operator_id::add:
				return "+";
			case instruction_operator_id::sub:
				return "-";
			case instruction_operator_id::mult:
				return "*";
			case instruction_operator_id::div:
				return "/";
			case instruction_operator_id::mod:
				return "%";
			case instruction_operator_id::and_:
				return "&";
			case instruction_operator_id::or_:
				return "|";
			case instruction_operator_id::xor_:
				return "^";
			case instruction_operator_id::sal:
				return "<<";
			case instruction_operator_id::sar:
				return ">>";
			default:
				break;
			}

			return "";
		}
	};
}

#endif /* !ELANG_INSTRUCTION_OPERATOR_ID_H */
