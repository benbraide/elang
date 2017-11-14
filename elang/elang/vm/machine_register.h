#pragma once

#ifndef ELANG_MACHINE_REGISTER_H
#define ELANG_MACHINE_REGISTER_H

#include <string>
#include <memory>

#include <boost/endian/buffers.hpp>

#include "../common/output_writer.h"

#include "machine_value_type_id.h"
#include "machine_register_error.h"

namespace elang::vm{
	class machine_register{
	public:
		typedef unsigned __int64 uint64_type;

		typedef machine_value_type_id value_type_id_type;
		typedef machine_register_error error_type;

		typedef std::size_t size_type;
		typedef std::shared_ptr<machine_register> ptr_type;

		typedef elang::common::output_writer output_writer_type;

		explicit machine_register(const std::string &name);

		explicit machine_register(const std::string &name, size_type size, char *buffer);

		machine_register(const machine_register &) = delete;
			
		machine_register &operator =(const machine_register &) = delete;

		virtual ~machine_register() = default;

		virtual value_type_id_type type_id() const;

		template <typename value_type>
		void write(value_type value){
			if (sizeof(value_type) != size_)
				throw error_type::size_mismatch;
			else//Write
				write_(reinterpret_cast<char *>(&value));
		}

		template <typename value_type>
		void inc(value_type value = static_cast<value_type>(1)){
			write(read<value_type>() + value);
		}

		template <typename value_type>
		void dec(value_type value = static_cast<value_type>(1)){
			write(read<value_type>() - value);
		}

		template <typename target_type>
		target_type read() const{
			if (sizeof(target_type) != size_)
				throw error_type::size_mismatch;
				
			auto value = target_type();
			read_(reinterpret_cast<char *>(&value));

			return value;

		}

		virtual uint64_type read_64bits() const;

		ptr_type alias(const std::string &name) const;

		ptr_type low(const std::string &name) const;

		ptr_type high(const std::string &name) const;

		void push_onto_stack();

		void pop_from_stack();

		const std::string &name() const;

		size_type size() const;

		void print(output_writer_type &writer);

	protected:
		void init_(size_type size, char *buffer);

		virtual void write_(const char *buffer){
#ifdef BOOST_BIG_ENDIAN
			for (auto i = 0u; i < size_; ++i)
				buffer_[i] = buffer[size_ - (i + 1)];
#else	/* Little endian */
			memcpy(buffer_, buffer, size_);
#endif	/* BOOST_BIG_ENDIAN */
		}

		virtual void read_(char *buffer) const{
#ifdef BOOST_BIG_ENDIAN
			for (auto i = 0u; i < size_; ++i)
				buffer[i] = buffer_[size_ - (i + 1)];
#else	/* Little endian */
			memcpy(buffer, buffer_, size_);
#endif	/* BOOST_BIG_ENDIAN */
		}

		std::string name_;
		size_type size_;
		char *buffer_;
	};

	template <class value_type>
	class basic_machine_register : public machine_register{
	public:
		typedef value_type value_type;

		explicit basic_machine_register(const std::string &name)
			: machine_register(name){
			machine_register::init_(sizeof(value_type), buffer_);
			write(value_type());
		}

		virtual value_type_id_type type_id() const override{
			return machine_value_type<value_type>::id;
		}

		virtual uint64_type read_64bits() const override{
			return static_cast<uint64_type>(read<value_type>());
		}

	private:
		char buffer_[sizeof(value_type)];
	};

	template <>
	class basic_machine_register<long double> : public machine_register{
	public:
		typedef long double value_type;

		explicit basic_machine_register(const std::string &name)
			: machine_register(name){
			machine_register::init_(sizeof(value_type), buffer_);
			write(value_type());
		}

		virtual value_type_id_type type_id() const override{
			return machine_value_type<value_type>::id;
		}

		virtual uint64_type read_64bits() const override{
			return static_cast<uint64_type>(read<value_type>());
		}

	private:
		char buffer_[sizeof(value_type)];
	};
}

#endif /* !ELANG_MACHINE_REGISTER_H */
