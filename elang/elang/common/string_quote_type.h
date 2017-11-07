#pragma once

#ifndef ELANG_STRING_QUOTE_TYPE_H
#define ELANG_STRING_QUOTE_TYPE_H

namespace elang::common{
	enum class string_quote_type{
		narrow,
		escaped_narrow,
		wide,
		escaped_wide,
	};
}

#endif /* !ELANG_STRING_QUOTE_TYPE_H */
