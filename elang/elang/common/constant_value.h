#pragma once

#ifndef ELANG_CONSTANT_VALUE_H
#define ELANG_CONSTANT_VALUE_H

namespace elang::common{
	enum class constant_value{
		nil,
		false_,
		true_,
		indeterminate,
		nullptr_,
		nan_,
		undefined_,
		infinite_,
	};
}

#endif /* !ELANG_CONSTANT_VALUE_H */
