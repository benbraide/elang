#pragma once

#ifndef ELANG_TYPE_INFO_H
#define ELANG_TYPE_INFO_H

#include <list>
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>

#include "../common/macro.h"
#include "../common/primitive_type_id.h"

#include "machine_value_type_id.h"

#define ELANG_TYPE_INFO_MIN_SCORE 0
#define ELANG_TYPE_INFO_MAX_SCORE 20

namespace elang::vm{
	class symbol_entry;

	class type_info : public std::enable_shared_from_this<type_info>{
	public:
		typedef std::size_t size_type;
		typedef std::shared_ptr<type_info> ptr_type;
		typedef std::shared_ptr<symbol_entry> symbol_ptr_type;

		typedef elang::common::primitive_type_id primitive_id_type;

		enum class attribute_type : unsigned int{
			nil					= (0 << 0x0000),
			const_				= (1 << 0x0000),
			ref_				= (1 << 0x0001),
			vref				= (1 << 0x0002),
		};

		struct entry_type{
			ptr_type value;
			attribute_type attributes;
		};

		explicit type_info(attribute_type attributes);

		virtual ptr_type reflect();

		virtual ptr_type clone(attribute_type attributes) const = 0;

		virtual machine_value_type_id id() const;

		virtual primitive_id_type primitive_id() const;

		virtual attribute_type attributes() const;

		virtual size_type size() const = 0;

		virtual type_info *underlying_type() const;

		virtual std::string mangle() const;

		virtual std::string mangle_as_parameter() const;

		virtual std::string mangle_attributes() const;

		virtual int score(const type_info &type) const = 0;

		virtual bool is_same(const type_info &type) const = 0;

		virtual bool is_same_unmodified(const type_info &type) const = 0;

		virtual bool is_same_object(const type_info &type) const;

		virtual bool is_compatible(const type_info &type) const;

		virtual bool is_null_pointer() const;

		virtual bool is_pointer() const;

		virtual bool is_array() const;

		virtual bool is_function() const;

		virtual bool is_void() const;

		virtual bool is_bool() const;

		virtual bool is_numeric() const;

		virtual bool is_integral() const;

		virtual bool is_const() const;

		virtual bool is_ref() const;

		virtual bool is_vref() const;

		virtual bool is_variadic() const;

		virtual bool is_optional() const;

	protected:
		virtual bool is_same_(const type_info &type) const;

		virtual bool is_compatible_(const type_info &type) const;

		virtual std::string mangle_() const = 0;

		virtual std::string mangle_attributes_(attribute_type attributes) const;

		attribute_type attributes_;
	};

	class primitive_type_info : public type_info{
	public:
		primitive_type_info(primitive_id_type id, attribute_type attributes);

		virtual ptr_type clone(attribute_type attributes) const override;

		virtual machine_value_type_id id() const override;

		virtual primitive_id_type primitive_id() const override;

		virtual size_type size() const override;

		virtual int score(const type_info &type) const override;

		virtual bool is_same(const type_info &type) const override;

		virtual bool is_same_unmodified(const type_info &type) const override;

		virtual bool is_compatible(const type_info &type) const override;

		virtual bool is_null_pointer() const override;

		virtual bool is_void() const override;

		virtual bool is_bool() const override;

		virtual bool is_numeric() const override;

		virtual bool is_integral() const override;

	protected:
		virtual std::string mangle_() const override;

		virtual bool is_same_id_(const type_info &type) const;

		virtual bool has_conversion_(const type_info &type) const;

		primitive_id_type id_;
	};

	class user_type_info : public type_info{
	public:
		user_type_info(symbol_ptr_type value, attribute_type attributes);

		virtual ptr_type clone(attribute_type attributes) const override;

		virtual size_type size() const override;

		virtual int score(const type_info &type) const override;

		virtual bool is_same(const type_info &type) const override;

		virtual bool is_same_unmodified(const type_info &type) const override;

		virtual bool is_compatible(const type_info &type) const override;

	protected:
		virtual std::string mangle_() const override;

		virtual bool is_same_symbol_(const type_info &type) const;

		virtual bool is_base_type_(const type_info &type) const;

		virtual bool has_conversion_(const type_info &type) const;

		symbol_ptr_type value_;
	};

	class pointer_type_info : public type_info{
	public:
		pointer_type_info(ptr_type value, attribute_type attributes);

		virtual ptr_type clone(attribute_type attributes) const override;

		virtual size_type size() const override;

		virtual type_info *underlying_type() const override;

		virtual int score(const type_info &type) const override;

		virtual bool is_same(const type_info &type) const override;

		virtual bool is_same_unmodified(const type_info &type) const override;

		virtual bool is_compatible(const type_info &type) const override;

		virtual bool is_pointer() const override;

	protected:
		virtual std::string mangle_() const override;

		virtual bool is_same_underlying_types_(const type_info &type) const;

		virtual bool has_conversion_(const type_info &type) const;

		ptr_type value_;
	};

	class array_type_info : public type_info{
	public:
		array_type_info(ptr_type value, size_type count, attribute_type attributes);

		virtual ptr_type clone(attribute_type attributes) const override;

		virtual size_type size() const override;

		virtual type_info *underlying_type() const override;

		virtual int score(const type_info &type) const override;

		virtual bool is_same(const type_info &type) const override;

		virtual bool is_same_unmodified(const type_info &type) const override;

		virtual bool is_array() const override;

	protected:
		virtual std::string mangle_() const override;

		ptr_type value_;
		size_type count_;
	};

	class function_type_info : public type_info{
	public:
		typedef std::vector<ptr_type> ptr_list_type;

		function_type_info(ptr_type return_type, const ptr_list_type &parameters, attribute_type attributes);

		function_type_info(ptr_type return_type, ptr_list_type &&parameters, attribute_type attributes);

		virtual ptr_type clone(attribute_type attributes) const override;

		virtual size_type size() const override;

		virtual int score(const type_info &type) const override;

		virtual bool is_same(const type_info &type) const override;

		virtual bool is_same_unmodified(const type_info &type) const override;

		virtual bool is_function() const override;

		bool is_same_return_type(const function_type_info &type) const;

		bool is_same_parameters(const function_type_info &type) const;

		ptr_type return_type() const;

		const ptr_list_type &parameters() const;

		std::string mangle_parameters() const;

	protected:
		virtual std::string mangle_() const override;

		ptr_type return_type_;
		ptr_list_type parameters_;
	};

	class variadic_type_info : public type_info{
	public:
		variadic_type_info(ptr_type value, attribute_type attributes);

		virtual ptr_type clone(attribute_type attributes) const override;

		virtual size_type size() const override;

		virtual type_info *underlying_type() const override;

		virtual int score(const type_info &type) const override;

		virtual bool is_same(const type_info &type) const override;

		virtual bool is_same_unmodified(const type_info &type) const override;

		virtual bool is_compatible(const type_info &type) const override;

		virtual bool is_variadic() const override;

		virtual bool is_optional() const override;

	protected:
		virtual std::string mangle_() const override;

		ptr_type value_;
	};

	class optional_type_info : public type_info{
	public:
		explicit optional_type_info(ptr_type value);

		virtual ptr_type clone(attribute_type attributes) const override;

		virtual machine_value_type_id id() const override;

		virtual primitive_id_type primitive_id() const override;

		virtual attribute_type attributes() const override;

		virtual size_type size() const override;

		virtual type_info *underlying_type() const override;

		virtual std::string mangle() const override;

		virtual std::string mangle_as_parameter() const override;

		virtual std::string mangle_attributes() const override;

		virtual int score(const type_info &type) const override;

		virtual bool is_same(const type_info &type) const override;

		virtual bool is_same_unmodified(const type_info &type) const override;

		virtual bool is_same_object(const type_info &type) const override;

		virtual bool is_compatible(const type_info &type) const override;

		virtual bool is_null_pointer() const override;

		virtual bool is_pointer() const override;

		virtual bool is_array() const override;

		virtual bool is_function() const override;

		virtual bool is_void() const override;

		virtual bool is_bool() const override;

		virtual bool is_numeric() const override;

		virtual bool is_integral() const override;

		virtual bool is_const() const override;

		virtual bool is_ref() const override;

		virtual bool is_vref() const override;

		virtual bool is_variadic() const override;

		virtual bool is_optional() const override;

	protected:
		virtual std::string mangle_() const override;

		ptr_type value_;
	};

	ELANG_MAKE_OPERATORS(type_info::attribute_type);
}

#endif /* !ELANG_TYPE_INFO_H */
