#pragma once

#ifndef ELANG_INSTRUCTION_OPERAND_ID_H
#define ELANG_INSTRUCTION_OPERAND_ID_H

namespace elang::easm::instruction{
	enum class operand_id{
		nil,
		constant_value,
		register_value,
		memory,
		expression,
		label,
		offset,
	};
}

#endif /* !ELANG_INSTRUCTION_OPERAND_ID_H */
