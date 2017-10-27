#pragma once

#ifndef ELANG_INSTRUCTION_ERROR_H
#define ELANG_INSTRUCTION_ERROR_H

namespace elang{
	namespace asm_{
		enum class instruction_error{
			nil,
			bad_operation,
			ambiguous_operation,
			bad_conversion,
			bad_identifier,
			bad_operand,
			section_redifinition,
			bad_instruction,
			no_section,
			no_main,
		};
	}
}

#endif /* !ELANG_INSTRUCTION_ERROR_H */
