#pragma once

#ifndef ELANG_MACRO_H
#define ELANG_MACRO_H

#define ELANG_MAKE_OPERATORS_(x, t)\
	inline constexpr x operator |(x l, x r){\
		return (x)((t)(l) | (t)(r));\
	}\
\
	inline constexpr x operator &(x l, x r){\
		return (x)((t)(l) & (t)(r));\
	}\
\
	inline constexpr x operator ~(x r){\
		return (x)(~(t)(r));\
	}\
\
	inline x operator |=(x &l, x r){\
		return (l = (x)((t)(l) | (t)(r)));\
	}\
\
	inline x operator &=(x &l, x r){\
		return (l = (x)((t)(l) & (t)(r)));\
	}\
\
	inline x operator <<(x &l, t r){\
		return (x)((t)(l) << (r));\
	}\
\
	inline x operator >>(x &l, t r){\
		return (x)((t)(l) >> (r));\
	}

#define ELANG_MAKE_OPERATORS(x) ELANG_MAKE_OPERATORS_(x, unsigned int)

#define ELANG_MAKE_OPERATORS_LL(x) ELANG_MAKE_OPERATORS_(x, unsigned long long)

#define ELANG_SET(source, target) ((source) |= (target))
#define ELANG_REMOVE(source, target) ((source) &= ~(target))
#define ELANG_FILTER(source, target) ((source) &= (target))

#define ELANG_SET_V(source, target) ((source) | (target))
#define ELANG_REMOVE_V(source, target) ((source) & ~(target))
#define ELANG_FILTER_V(source, target) ((source) & (target))

#define ELANG_IS(source, target) (((source) & (target)) == (target))
#define ELANG_IS_ANY(source, target) (((source) & (target)) != static_cast<decltype(target)>(0))

#endif /* !ELANG_MACRO_H */
