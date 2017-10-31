#pragma once

#ifndef ELANG_AST_H
#define ELANG_AST_H

#include <string>
#include <vector>

#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/fusion/adapted/mpl.hpp>

#include <boost/spirit/home/x3/support/ast/position_tagged.hpp>
#include <boost/spirit/home/x3/support/ast/variant.hpp>
#include <boost/spirit/home/x3.hpp>
#include <boost/optional.hpp>
#include <boost/variant.hpp>

#define ELANG_AST_JOIN_(x, y) x ## y
#define ELANG_AST_JOIN(x, y) ELANG_AST_JOIN_(x, y)

#define ELANG_AST_PREFIX									\
namespace x3 = boost::spirit::x3;

#define ELANG_AST_BEGIN namespace elang::grammar::ast{		\
															\
ELANG_AST_PREFIX

#define ELANG_AST_END }

#define ELANG_AST_DECLARE_SINGLE(name, type)				\
struct name{												\
	typedef type value_type;								\
	type value;												\
};

#define ELANG_AST_DECLARE_SINGLE_WPOS(name, type)			\
struct name : x3::position_tagged{							\
	typedef type value_type;								\
	value_type value;										\
};

#define ELANG_AST_DECLARE_SINGLE_VARIANT(name, ...)			\
struct name{												\
	typedef boost::variant<__VA_ARGS__> value_type;			\
	value_type value;										\
};

#define ELANG_AST_DECLARE_SINGLE_VARIANT_WPOS(name, ...)	\
struct name : x3::position_tagged{							\
	typedef boost::variant<__VA_ARGS__> value_type;			\
	value_type value;										\
};

#define ELANG_AST_ADAPT_SINGLE(name)						\
BOOST_FUSION_ADAPT_STRUCT(									\
	elang::grammar::ast::name,								\
	(elang::grammar::ast::name::value_type, value)			\
)

#endif /* !ELANG_AST_H */
