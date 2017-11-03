#pragma once

#ifndef ELANG_INSTRUCTION_ERROR_H
#define ELANG_INSTRUCTION_ERROR_H

namespace elang{
	namespace easm{
		enum class instruction_error{
			nil,
			bad_operation,
			ambiguous_operation,
			bad_conversion,
			bad_identifier,
			bad_operand,
			bad_operand_count,
			operands_type_mismatch,
			section_redifinition,
			bad_instruction,
			label_not_found,
			no_section,
			no_main,
		};
	}
}

#endif /* !ELANG_INSTRUCTION_ERROR_H */
