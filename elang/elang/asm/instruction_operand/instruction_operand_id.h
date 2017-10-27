#pragma once

#ifndef ELANG_INSTRUCTION_OPERAND_ID_H
#define ELANG_INSTRUCTION_OPERAND_ID_H

namespace elang{
	namespace asm_{
		namespace instruction{
			enum class operand_id{
				nil,
				constant_value,
				register_value,
				memory,
				expression,
				label,
			};
		}
	}
}

#endif /* !ELANG_INSTRUCTION_OPERAND_ID_H */
