#pragma once

#ifndef ELANG_NUMERIC_LITERAL_SUFFIX_H
#define ELANG_NUMERIC_LITERAL_SUFFIX_H

namespace elang::common{
	enum class numeric_literal_suffix{
		int8,
		uint8,
		int16,
		uint16,
		int32,
		uint32,
		int64,
		uint64,
		float_,
		double_,
		long_double_,
	};
}

#endif /* !ELANG_NUMERIC_LITERAL_SUFFIX_H */
