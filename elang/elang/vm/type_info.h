#pragma once

#ifndef ELANG_TYPE_INFO_H
#define ELANG_TYPE_INFO_H

#include <list>
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>

#include "../common/macro.h"

#include "machine_value_type_id.h"

namespace elang::vm{
	enum class type_info_error{
		nil,
		unreachable,
		not_applicable,
		redefinition,
		unique_return_type_only,
	};

	class type_info{
	public:
		typedef type_info_error error_type;

		typedef std::size_t size_type;
		typedef std::shared_ptr<type_info> ptr_type;

		enum class attribute_type : unsigned int{
			nil					= (0 << 0x0000),
			static_				= (1 << 0x0000),
			tls					= (1 << 0x0001),
			private_			= (1 << 0x0002),
			protected_			= (1 << 0x0003),
		};

		struct entry_type{
			ptr_type value;
			attribute_type attributes;
		};

		virtual machine_value_type_id id() const;

		virtual size_type size() const = 0;

		virtual type_info *underlying_type() const;

		virtual entry_type *find(const std::string &key) const;

		virtual bool is_same(const type_info &type) const = 0;

		virtual bool is_compatible(const type_info &type) const;

		virtual bool is_primitive() const;

		virtual bool is_integral() const;

		virtual bool is_pointer() const;

		virtual bool is_array() const;

		virtual bool is_function() const;

		virtual bool is_void() const;

		virtual bool is_bool() const;

		virtual bool is_storage() const;

		virtual bool is_ref() const;

		virtual bool is_vref() const;
	};

	class primitive_type_info : public type_info{
	public:
		enum class index_type{
			int8_,
			uint8_,
			int16_,
			uint16_,
			int32_,
			uint32_,
			int64_,
			uint64_,
			float_,
		};

		explicit primitive_type_info(index_type index);

		virtual machine_value_type_id id() const override;

		virtual size_type size() const override;

		virtual bool is_same(const type_info &type) const override;

		virtual bool is_compatible(const type_info &type) const override;

		virtual bool is_primitive() const override;

		virtual bool is_integral() const override;

	private:
		index_type index_;
	};

	class pointer_type_info : public type_info{
	public:
		explicit pointer_type_info(ptr_type value);

		virtual size_type size() const override;

		virtual type_info *underlying_type() const override;

		virtual bool is_same(const type_info &type) const override;

		virtual bool is_compatible(const type_info &type) const override;

		virtual bool is_pointer() const override;

	private:
		ptr_type value_;
	};

	class array_type_info : public type_info{
	public:
		array_type_info(ptr_type value, size_type count);

		virtual size_type size() const override;

		virtual type_info *underlying_type() const override;

		virtual bool is_same(const type_info &type) const override;

		virtual bool is_array() const override;

	private:
		ptr_type value_;
		size_type count_;
	};

	class function_type_info : public type_info{
	public:
		typedef std::vector<ptr_type> ptr_list_type;

		function_type_info(ptr_type return_type, const ptr_list_type &parameters);

		function_type_info(ptr_type return_type, ptr_list_type &&parameters);

		virtual size_type size() const override;

		virtual bool is_same(const type_info &type) const override;

		virtual bool is_function() const override;

		bool is_same_return_type(const function_type_info &type) const;

		bool is_same_parameters(const function_type_info &type) const;

	private:
		ptr_type return_type_;
		ptr_list_type parameters_;
	};

	class function_list_type_info : public type_info{
	public:
		typedef std::list<entry_type> ptr_list_type;

		virtual size_type size() const override;

		virtual bool is_same(const type_info &type) const override;

		virtual bool is_function() const override;

		void add(ptr_type value, attribute_type attributes);

	private:
		void add_(ptr_type value, attribute_type attributes);

		function_type_info *get_same_parameters_(const function_type_info &function_type) const;

		ptr_list_type list_;
	};

	class void_type_info : public type_info{
	public:
		virtual size_type size() const override;

		virtual bool is_same(const type_info &type) const override;

		virtual bool is_void() const override;
	};

	class bool_type_info : public type_info{
	public:
		virtual size_type size() const override;

		virtual bool is_same(const type_info &type) const override;

		virtual bool is_bool() const override;
	};

	class storage_type_info : public type_info{
	public:
		typedef std::unordered_map<std::string, entry_type> map_type;

		storage_type_info();

		explicit storage_type_info(const map_type &map);

		explicit storage_type_info(map_type &&map);

		virtual size_type size() const override;

		virtual entry_type *find(const std::string &key) const override;

		virtual bool is_same(const type_info &type) const override;

		virtual bool is_storage() const override;

		virtual void add(const std::string &key, ptr_type value, attribute_type attributes);

	protected:
		virtual void compute_size_();

		virtual void update_size_(size_type size);

		function_list_type_info *get_function_list_(const std::string &key);

		map_type map_;
		size_type size_;
	};

	using struct_type_info = storage_type_info;

	class union_type_info : public storage_type_info{
	public:
		template <typename... args_types>
		explicit union_type_info(args_types &&... args)
			: storage_type_info(std::forward<args_types>(args)...){}

	protected:
		virtual void compute_size_() override;

		virtual void update_size_(size_type size) override;
	};

	class proxy_type_info : public type_info{
	public:
		explicit proxy_type_info(ptr_type value);

		virtual machine_value_type_id id() const override;

		virtual size_type size() const override;

		virtual type_info *underlying_type() const override;

		virtual entry_type *find(const std::string &key) const override;

		virtual bool is_same(const type_info &type) const override;

		virtual bool is_compatible(const type_info &type) const override;

		virtual bool is_primitive() const override;

		virtual bool is_integral() const override;

		virtual bool is_pointer() const override;

		virtual bool is_array() const override;

		virtual bool is_function() const override;

		virtual bool is_void() const override;

		virtual bool is_bool() const override;

		virtual bool is_storage() const override;

		virtual bool is_ref() const override;

		virtual bool is_vref() const override;

	private:
		ptr_type value_;
	};

	class ref_type_info : public proxy_type_info{
	public:
		template <typename... args_types>
		explicit ref_type_info(args_types &&... args)
			: proxy_type_info(std::forward<args_types>(args)...){}

		virtual bool is_ref() const override;
	};

	class vref_type_info : public proxy_type_info{
	public:
		template <typename... args_types>
		explicit vref_type_info(args_types &&... args)
			: proxy_type_info(std::forward<args_types>(args)...){}

		virtual bool is_vref() const override;
	};

	using typedef_type_info = proxy_type_info;

	ELANG_MAKE_OPERATORS(type_info::attribute_type);
}

#endif /* !ELANG_TYPE_INFO_H */
