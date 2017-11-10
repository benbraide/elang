#pragma once

#ifndef ELANG_SYMBOLS_TABLE_H
#define ELANG_SYMBOLS_TABLE_H

#include <string>
#include <memory>
#include <vector>
#include <unordered_map>

#include "../common/macro.h"
#include "../common/primitive_type_id.h"

#include "type_info.h"

namespace elang::vm{
	enum class symbol_entry_id{
		namespace_,
		type,
		variable,
		function,
	};

	enum class symbol_entry_attribute : unsigned int{
		nil					= (0 << 0x0000),
		static_				= (1 << 0x0000),
		tls					= (1 << 0x0001),
		private_			= (1 << 0x0002),
		protected_			= (1 << 0x0003),
		undefined_			= (1 << 0x0004),
	};

	class symbol_entry : public std::enable_shared_from_this<symbol_entry>{
	public:
		typedef symbol_entry_id id_type;
		typedef symbol_entry_attribute attribute_type;

		typedef type_info::ptr_type type_info_ptr_type;

		typedef std::size_t size_type;
		typedef std::shared_ptr<symbol_entry> ptr_type;

		symbol_entry(symbol_entry *parent, attribute_type attributes);

		virtual ptr_type reflect();

		virtual id_type id() const = 0;

		virtual attribute_type attributes() const;

		virtual symbol_entry *parent() const;

		virtual size_type size() const;

		virtual type_info_ptr_type type() const;

		virtual std::string mangle() const = 0;

		virtual bool is(const std::string &name) const = 0;

		virtual void define();

	protected:
		symbol_entry *parent_;
		attribute_type attributes_;
	};

	class primitive_type_symbol_entry : public symbol_entry{
	public:
		typedef elang::common::primitive_type_id primitive_type_id_type;

		primitive_type_symbol_entry(symbol_entry *parent, primitive_type_id_type primitive_type_id);

		virtual id_type id() const override;

		virtual size_type size() const override;

		virtual std::string mangle() const override;

		virtual bool is(const std::string &name) const override;

		primitive_type_id_type primitive_type_id() const;

	protected:
		primitive_type_id_type primitive_type_id_;
	};

	class named_symbol_entry : public symbol_entry{
	public:
		template <typename... args_types>
		named_symbol_entry(const std::string &name, args_types &&... args)
			: symbol_entry(std::forward<args_types>(args)...), name_(name){}

		virtual std::string mangle() const override;

		virtual bool is(const std::string &name) const override;

	protected:
		std::string name_;
	};

	class variable_symbol_entry : public named_symbol_entry{
	public:
		template <typename... args_types>
		variable_symbol_entry(type_info_ptr_type type, args_types &&... args)
			: named_symbol_entry(std::forward<args_types>(args)...){}

		virtual id_type id() const override;

		virtual type_info_ptr_type type() const override;

	protected:
		type_info_ptr_type type_;
	};

	class function_symbol_entry : public variable_symbol_entry{
	public:
		template <typename... args_types>
		function_symbol_entry(args_types &&... args)
			: variable_symbol_entry(std::forward<args_types>(args)...){}

		virtual id_type id() const override;

		virtual std::string mangle() const override;
	};

	class function_list_symbol_entry : public named_symbol_entry{
	public:
		typedef std::vector<ptr_type> ptr_list_type;
		typedef std::vector<type_info_ptr_type> type_info_ptr_list_type;

		template <typename... args_types>
		function_list_symbol_entry(args_types &&... args)
			: named_symbol_entry(std::forward<args_types>(args)...){}

		virtual id_type id() const override;

		virtual std::string mangle() const override;

		virtual void add(ptr_type value);

		virtual symbol_entry *find(const type_info_ptr_list_type &parameter_types) const;

		virtual symbol_entry *resolve(const type_info_ptr_list_type &parameter_types) const;

	protected:
		void add_(function_symbol_entry &value);

		ptr_list_type list_;
	};

	class storage_symbol_entry : public named_symbol_entry{
	public:
		typedef std::unordered_map<std::string, ptr_type> map_type;

		template <typename... args_types>
		storage_symbol_entry(args_types &&... args)
			: named_symbol_entry(std::forward<args_types>(args)...){}

		virtual void add(const std::string &key, ptr_type value);

		virtual symbol_entry *find(const std::string &key) const;

	private:
		void add_(const std::string &key, function_symbol_entry &value);

		function_list_symbol_entry *get_function_list_(const std::string &key);

		map_type map_;
	};

	class type_symbol_entry : public storage_symbol_entry{
	public:
		template <typename... args_types>
		type_symbol_entry(args_types &&... args)
			: storage_symbol_entry(std::forward<args_types>(args)...), size_(0){}

		virtual id_type id() const override;

		virtual size_type size() const override;

		virtual void add(const std::string &key, ptr_type value) override;

	protected:
		virtual void update_size_(symbol_entry &value) = 0;

		size_type size_;
	};

	class enum_type_symbol_entry : public type_symbol_entry{
	public:
		template <typename... args_types>
		enum_type_symbol_entry(size_type size, args_types &&... args)
			: type_symbol_entry(std::forward<args_types>(args)...){
			size_ = size;
		}

	protected:
		virtual void update_size_(symbol_entry &value) override;
	};

	class union_type_symbol_entry : public type_symbol_entry{
	public:
		template <typename... args_types>
		union_type_symbol_entry(args_types &&... args)
			: type_symbol_entry(std::forward<args_types>(args)...){}

	protected:
		virtual void update_size_(symbol_entry &value) override;
	};

	class struct_type_symbol_entry : public type_symbol_entry{
	public:
		template <typename... args_types>
		struct_type_symbol_entry(args_types &&... args)
			: type_symbol_entry(std::forward<args_types>(args)...){}

	protected:
		virtual void update_size_(symbol_entry &value) override;
	};

	class class_type_symbol_entry : public type_symbol_entry{
	public:
		struct base_info_type{
			ptr_type value;
			attribute_type attributes;
		};

		typedef std::vector<std::string> string_list_type;
		typedef std::unordered_map<std::string, base_info_type> base_map_type;

		template <typename... args_types>
		class_type_symbol_entry(args_types &&... args)
			: type_symbol_entry(std::forward<args_types>(args)...){}

		virtual symbol_entry *find(const std::string &key) const override;

		virtual symbol_entry *find_inside_this(const std::string &key) const;

		virtual symbol_entry *find_base_or_this(const std::string &key);

		virtual void add_base(const std::string &key, ptr_type value, attribute_type attributes);

		virtual bool is_direct_base(symbol_entry &value) const;

		virtual bool is_base(symbol_entry &value) const;

	protected:
		virtual void update_size_(symbol_entry &value) override;

		string_list_type base_order_list_;
		base_map_type base_map_;
	};

	class namespace_symbol_entry : public storage_symbol_entry{
	public:
		template <typename... args_types>
		namespace_symbol_entry(args_types &&... args)
			: storage_symbol_entry(std::forward<args_types>(args)...){}

		virtual id_type id() const override;

		virtual std::string mangle() const override;
	};

	ELANG_MAKE_OPERATORS(symbol_entry_attribute)
}

#endif /* !ELANG_SYMBOLS_TABLE_H */
