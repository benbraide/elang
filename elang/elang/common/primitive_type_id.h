#pragma once

#ifndef ELANG_PRIMITIVE_TYPE_ID_H
#define ELANG_PRIMITIVE_TYPE_ID_H

namespace elang::common{
	enum class primitive_type_id{
		auto_,
		void_,
		nullptr_,
		bool_,
		char_,
		wchar_,
		int8_,
		uint8_,
		int16_,
		uint16_,
		int32_,
		uint32_,
		int64_,
		uint64_,
		float_,
		pointer,
	};
}

#endif /* !ELANG_PRIMITIVE_TYPE_ID_H */
