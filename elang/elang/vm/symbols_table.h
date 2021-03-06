#pragma once

#ifndef ELANG_SYMBOLS_TABLE_H
#define ELANG_SYMBOLS_TABLE_H

#include <any>
#include <string>
#include <memory>
#include <vector>
#include <unordered_map>

#include "../common/macro.h"
#include "../common/primitive_type_id.h"

#include "../asm/instruction_section.h"
#include "../asm/instruction_operand/instruction_operand_base.h"

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
		static_const		= (1 << 0x0005),
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

		virtual bool is_storage() const;

		virtual void define();

	protected:
		symbol_entry *parent_;
		attribute_type attributes_;
	};

	class named_symbol_entry : public symbol_entry{
	public:
		template <typename... args_types>
		explicit named_symbol_entry(const std::string &name, args_types &&... args)
			: symbol_entry(std::forward<args_types>(args)...), name_(name){}

		virtual std::string mangle() const override;

		virtual bool is(const std::string &name) const override;

	protected:
		std::string name_;
	};

	class function_symbol_entry;
	class function_list_symbol_entry;

	class variable_symbol_entry : public named_symbol_entry{
	public:
		typedef elang::easm::instruction::operand_base::ptr_type instruction_operand_ptr_type;

		template <typename... args_types>
		explicit variable_symbol_entry(args_types &&... args)
			: named_symbol_entry(std::forward<args_types>(args)...){}

		virtual id_type id() const override;

		virtual size_type size() const override;

		virtual type_info_ptr_type type() const override;

		virtual void add_reference();

		virtual size_type reference_count() const;

		virtual void set_stack_offset(size_type value);

		virtual size_type stack_offset() const;

		template <typename value_type>
		void set_initialization(const value_type &value){
			initialization_ = value;
		}

		template <typename target_type>
		const std::decay_t<target_type> &get_initialization() const{
			return std::any_cast<const std::decay_t<target_type> &>(initialization_);
		}

		virtual bool has_initialization() const{
			return initialization_.has_value();
		}

	protected:
		std::any initialization_;
	};

	class basic_variable_symbol_entry : public variable_symbol_entry{
	public:
		template <typename... args_types>
		explicit basic_variable_symbol_entry(type_info_ptr_type type, args_types &&... args)
			: variable_symbol_entry(std::forward<args_types>(args)...), type_(type), stack_offset_(static_cast<size_type>(-1)){}

		virtual type_info_ptr_type type() const override;

		virtual void add_reference();

		virtual size_type reference_count() const;

		virtual void set_stack_offset(size_type value);

		virtual size_type stack_offset() const;

	protected:
		type_info_ptr_type type_;
		size_type stack_offset_;
		size_type ref_count_;
	};

	class reference_variable_symbol_entry : public variable_symbol_entry{
	public:
		template <typename... args_types>
		explicit reference_variable_symbol_entry(variable_symbol_entry &target, args_types &&... args)
			: variable_symbol_entry(std::forward<args_types>(args)...), target_(&target){
			//type_ = target_->type()->clone(attributes_);
		}

		virtual type_info_ptr_type type() const override;

		virtual void add_reference() override;

		virtual size_type reference_count() const override;

		virtual void set_stack_offset(size_type value) override;

		virtual size_type stack_offset() const override;

	protected:
		variable_symbol_entry *target_;
		type_info_ptr_type type_;
	};

	class storage_symbol_entry : public named_symbol_entry{
	public:
		typedef std::unordered_map<std::string, ptr_type> map_type;
		typedef std::list<ptr_type> list_type;

		template <typename... args_types>
		explicit storage_symbol_entry(args_types &&... args)
			: named_symbol_entry(std::forward<args_types>(args)...){}

		virtual bool is_storage() const override;

		virtual void add(const std::string &key, ptr_type value);

		virtual variable_symbol_entry *add_variable(const std::string &name, type_info_ptr_type type, attribute_type attributes);

		virtual variable_symbol_entry *add_reference_variable(const std::string &name, variable_symbol_entry &target, attribute_type attributes);

		virtual symbol_entry *find(const std::string &key) const;

		virtual symbol_entry *find_storage(const std::string &key) const;

		virtual symbol_entry *find_storage_or_any(const std::string &key) const;

		virtual size_type get_stack_offset(size_type new_size);

	private:
		void add_(const std::string &key, function_symbol_entry &value);

		function_list_symbol_entry *get_function_list_(const std::string &key);

		map_type map_;
		list_type order_list_;
	};

	class function_symbol_entry : public storage_symbol_entry{
	public:
		typedef std::vector<type_info_ptr_type> type_info_ptr_list_type;
		typedef std::list<variable_symbol_entry *> variable_symbol_list_type;

		template <typename... args_types>
		explicit function_symbol_entry(type_info_ptr_type return_type, args_types &&... args)
			: storage_symbol_entry(std::forward<args_types>(args)...), return_type_(return_type){}

		virtual id_type id() const override;

		virtual size_type size() const override;

		virtual type_info_ptr_type type() const override;

		virtual std::string mangle() const override;

		virtual bool is_storage() const override;

		virtual size_type get_stack_offset(size_type new_size) override;

		virtual variable_symbol_entry *add_parameter(const std::string &name, type_info_ptr_type type);

		virtual int score(const type_info_ptr_list_type &parameter_types) const;

	protected:
		virtual type_info_ptr_type make_type_() const;

		type_info_ptr_type return_type_;
		mutable type_info_ptr_type type_;
		size_type size_;
		variable_symbol_list_type parameter_order_list_;
	};

	class function_list_symbol_entry : public named_symbol_entry{
	public:
		typedef std::vector<type_info_ptr_type> type_info_ptr_list_type;
		typedef std::unordered_map<std::string, ptr_type> ptr_map_type;

		template <typename... args_types>
		explicit function_list_symbol_entry(args_types &&... args)
			: named_symbol_entry(std::forward<args_types>(args)...){}

		virtual id_type id() const override;

		virtual std::string mangle() const override;

		virtual void add(ptr_type value);

		virtual symbol_entry *resolve(const type_info_ptr_list_type &parameter_types, int *score = nullptr) const;

	protected:
		void add_(function_symbol_entry &value);

		function_symbol_entry *find_(const std::string &value) const;

		ptr_map_type map_;
	};

	class type_symbol_entry : public storage_symbol_entry{
	public:
		template <typename... args_types>
		explicit type_symbol_entry(args_types &&... args)
			: storage_symbol_entry(std::forward<args_types>(args)...), size_(0){}

		virtual id_type id() const override;

		virtual size_type size() const override;

	protected:
		size_type size_;
	};

	class enum_type_symbol_entry : public type_symbol_entry{
	public:
		template <typename... args_types>
		explicit enum_type_symbol_entry(size_type size, args_types &&... args)
			: type_symbol_entry(std::forward<args_types>(args)...){
			size_ = size;
		}
	};

	class union_type_symbol_entry : public type_symbol_entry{
	public:
		template <typename... args_types>
		explicit union_type_symbol_entry(args_types &&... args)
			: type_symbol_entry(std::forward<args_types>(args)...){}

		virtual size_type get_stack_offset(size_type new_size) override;
	};

	class struct_type_symbol_entry : public type_symbol_entry{
	public:
		template <typename... args_types>
		explicit struct_type_symbol_entry(args_types &&... args)
			: type_symbol_entry(std::forward<args_types>(args)...){}

		virtual size_type get_stack_offset(size_type new_size) override;
	};

	class class_type_symbol_entry : public type_symbol_entry{
	public:
		struct base_info_type{
			class_type_symbol_entry *value;
			attribute_type attributes;
		};

		typedef std::vector<std::string> string_list_type;
		typedef std::unordered_map<std::string, base_info_type> base_map_type;

		template <typename... args_types>
		explicit class_type_symbol_entry(args_types &&... args)
			: type_symbol_entry(std::forward<args_types>(args)...){}

		virtual size_type get_stack_offset(size_type new_size) override;

		virtual symbol_entry *find(const std::string &key) const override;

		virtual symbol_entry *find_inside_this(const std::string &key) const;

		virtual symbol_entry *find_base_or_this(const std::string &key);

		virtual void add_base(const std::string &key, class_type_symbol_entry *value, attribute_type attributes);

		virtual bool is_direct_base(symbol_entry &value) const;

		virtual bool is_base(symbol_entry &value) const;

	protected:
		string_list_type base_order_list_;
		base_map_type base_map_;
	};

	class unnamed_class_type_symbol_entry : public class_type_symbol_entry{
	public:
		template <typename... args_types>
		explicit unnamed_class_type_symbol_entry(args_types &&... args)
			: class_type_symbol_entry(std::forward<args_types>(args)...){}

		virtual std::string mangle() const override;
	};

	class namespace_symbol_entry : public storage_symbol_entry{
	public:
		template <typename... args_types>
		explicit namespace_symbol_entry(args_types &&... args)
			: storage_symbol_entry(std::forward<args_types>(args)...){}

		virtual id_type id() const override;
	};

	class global_namespace_symbol_entry : public namespace_symbol_entry{
	public:
		template <typename... args_types>
		explicit global_namespace_symbol_entry(args_types &&... args)
			: namespace_symbol_entry("_G", std::forward<args_types>(args)...){}

		virtual std::string mangle() const override;
	};

	ELANG_MAKE_OPERATORS(symbol_entry_attribute)
}

#endif /* !ELANG_SYMBOLS_TABLE_H */
