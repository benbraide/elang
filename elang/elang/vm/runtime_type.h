#pragma once

#ifndef ELANG_RUNTIME_TYPE_H
#define ELANG_RUNTIME_TYPE_H

#include <string>
#include <memory>
#include <unordered_map>

#include "../common/output_writer.h"

#include "runtime_error.h"

namespace elang::vm{
	class runtime_type{
	public:
		typedef std::size_t size_type;
		typedef std::shared_ptr<runtime_type> ptr_type;

		typedef elang::common::output_writer writer_type;

		runtime_type(const std::string &name, runtime_type *parent)
			: name_(name), parent_(parent){}

		virtual ~runtime_type() = default;

		virtual size_type size() const = 0;

		virtual runtime_type *find(const std::string &key) const = 0;

		virtual runtime_type *parent() const{
			return parent_;
		}
		
		virtual void print(writer_type &writer) const{
			if (parent_ != nullptr){
				parent_->print(writer);
				writer.write(".");
			}

			writer.write(name_);
		}

	protected:
		std::string name_;
		runtime_type *parent_;
	};

	class scalar_runtime_type : public runtime_type{
	public:
		scalar_runtime_type(const std::string &name, runtime_type *parent, size_type size)
			: runtime_type(name, parent), size_(size){}

		virtual ~scalar_runtime_type() = default;

		virtual size_type size() const override{
			return size_;
		}

		virtual runtime_type *find(const std::string &key) const override{
			return nullptr;
		}

	private:
		size_type size_;
	};

	class compound_runtime_type : public runtime_type{
	public:
		typedef std::unordered_map<std::string, ptr_type> map_type;

		compound_runtime_type(const std::string &name, runtime_type *parent)
			: runtime_type(name, parent), size_(0){}

		virtual ~compound_runtime_type() = default;

		virtual size_type size() const override{
			return size_;
		}

		virtual runtime_type *find(const std::string &key) const override{
			auto entry = map_.find(key);
			return ((entry == map_.end()) ? nullptr : entry->second.get());
		}

		virtual void add(const std::string &key, ptr_type value){
			if (map_.find(key) == map_.end())
				size_ += (map_[key] = value)->size();
			else//Error
				throw runtime_error::type_redefinition;
		}

	private:
		size_type size_;
		map_type map_;
	};
}

#endif /* !ELANG_RUNTIME_TYPE_H */
