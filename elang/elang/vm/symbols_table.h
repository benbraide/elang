#pragma once

#ifndef ELANG_SYMBOLS_TABLE_H
#define ELANG_SYMBOLS_TABLE_H

#include <string>
#include <memory>
#include <unordered_map>

namespace elang::vm{
	enum class symbol_entry_id{
		namespace_,
		type,
		variable,
		function,
	};

	enum class symbol_error{
		nil,
		undefined,
		redifinition,
		uninitialized,
		unreferenced,
		not_storage,
		not_type,
	};

	class symbol_entry{
	public:
		typedef symbol_entry_id id_type;
		typedef symbol_error error_type;

		typedef std::shared_ptr<symbol_entry> ptr_type;

		typedef std::vector<std::string>::const_iterator iterator_type;

		symbol_entry(symbol_entry *parent, const std::string &name)
			: name_(name), parent_(parent){}

		virtual id_type id() const = 0;

		virtual void add(const std::string &key, ptr_type value){
			throw error_type::not_storage;
		}

		virtual void add(ptr_type value){
			throw error_type::redifinition;
		}

		virtual symbol_entry *find(iterator_type key, iterator_type end) const{
			throw error_type::not_storage;
		}

		virtual std::string qualified_name() const{
			return ((parent_ == nullptr) ? name_ : (parent_->qualified_name() + "$" + name_));
		}

		symbol_entry *parent() const{
			return parent_;
		}

		const std::string &name() const{
			return name_;
		}

	protected:
		symbol_entry *parent_;
		std::string name_;
	};

	class variable_symbol_entry : public symbol_entry{
	public:
		explicit variable_symbol_entry(const std::string &name)
			: symbol_entry(name){}

		virtual id_type id() const override{
			return id_type::variable;
		}
	};

	class storage_symbol_entry : public symbol_entry{
	public:
		typedef std::unordered_map<std::string, ptr_type> map_type;

		storage_symbol_entry(symbol_entry *parent, const std::string &name, id_type id)
			: symbol_entry(parent, name), id_(id){}

		virtual id_type id() const override{
			return id_;
		}

		virtual void add(const std::string &key, ptr_type value) override{
			auto entry = map_.find(key);
			if (entry == map_.end())
				map_[key] = value;
			else//Merge value -- requires function
				entry->second->add(value);
		}

		virtual symbol_entry *find(iterator_type key, iterator_type end) const override{
			if (key == end)//Match
				return const_cast<storage_symbol_entry *>(this);

			auto entry = map_.find(*key);
			return ((entry == map_.end()) ? nullptr : entry->second->find(++key, end));
		}

	private:
		id_type id_;
		map_type map_;
	};
}

#endif /* !ELANG_SYMBOLS_TABLE_H */
