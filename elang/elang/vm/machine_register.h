#pragma once

#ifndef ELANG_MACHINE_REGISTER_H
#define ELANG_MACHINE_REGISTER_H

#include <string>
#include <memory>

#include <boost/endian/buffers.hpp>

#include "../common/output_writer.h"

#include "machine_value_type_id.h"
#include "machine_register_error.h"

namespace elang{
	namespace vm{
		class machine_register{
		public:
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
					write_(value, std::bool_constant<std::is_integral<value_type>::value>());
			}

			template <typename value_type>
			void inc(value_type value = static_cast<value_type>(1)){
				write(read<value_type_id_type>() + value);
			}

			template <typename value_type>
			void dec(value_type value = static_cast<value_type>(1)){
				write(read<value_type_id_type>() - value);
			}

			template <typename target_type>
			target_type read() const{
				if (sizeof(target_type) != size_)
					throw error_type::size_mismatch;
				return read_<target_type>(std::bool_constant<std::is_integral<target_type>::value>());
			}

			ptr_type alias(const std::string &name) const;

			ptr_type low(const std::string &name) const;

			ptr_type high(const std::string &name) const;

			void push_onto_stack();

			void pop_from_stack();

			friend output_writer_type &operator <<(output_writer_type &writer, machine_register &reg){
				return (writer << reg.name_);
			}

		protected:
			void init_(size_type size, char *buffer);

			template <typename value_type>
			void write_(value_type value, std::true_type){
				switch (size_){
				case 1u:
					boost::endian::detail::store_little_endian<value_type, 1u>(buffer_, value);
					break;
				case 2u:
					boost::endian::detail::store_little_endian<value_type, 2u>(buffer_, value);
					break;
				case 4u:
					boost::endian::detail::store_little_endian<value_type, 4u>(buffer_, value);
					break;
				case 8u:
					boost::endian::detail::store_little_endian<value_type, 8u>(buffer_, value);
					break;
				default:
					throw error_type::size_mismatch;
					break;
				}
			}

			template <typename value_type>
			void write_(value_type value, std::false_type){
				do_write_(reinterpret_cast<char *>(&value));
			}

			virtual void do_write_(const char *buffer){
				throw error_type::type_mismatch;
			}

			template <typename target_type>
			target_type read_(std::true_type) const{
				switch (size_){
				case 1u:
					return boost::endian::detail::load_little_endian<target_type, 1u>(buffer_);
				case 2u:
					return boost::endian::detail::load_little_endian<target_type, 2u>(buffer_);
				case 4u:
					return boost::endian::detail::load_little_endian<target_type, 4u>(buffer_);
				case 8u:
					return boost::endian::detail::load_little_endian<target_type, 8u>(buffer_);
				default:
					break;
				}

				throw error_type::size_mismatch;
			}

			template <typename target_type>
			target_type read_(std::false_type) const{
				auto value = target_type();
				do_read_(reinterpret_cast<char *>(&value));
				return value;
			}

			virtual void do_read_(char *buffer) const{
				throw error_type::type_mismatch;
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

		private:
			virtual void do_write_(const char *buffer) override{
#ifdef BOOST_BIG_ENDIAN
				std::reverse_copy(buffer, buffer + size_, buffer_);
#else	/* Little endian */
				std::copy(buffer, buffer + size_, buffer_);
#endif	/* BOOST_BIG_ENDIAN */
			}

			virtual void do_read_(char *buffer) const override{
#ifdef BOOST_BIG_ENDIAN
				std::reverse_copy(buffer_, buffer_ + size_, buffer);
				for (auto i = 0u; i < size_; ++i)
					buffer[i] = buffer_[size_ - (i + 1)];
#else	/* Little endian */
				memcpy(buffer, buffer_, size_);
#endif	/* BOOST_BIG_ENDIAN */
			}

			char buffer_[sizeof(value_type)];
		};
	}
}

#endif /* !ELANG_MACHINE_REGISTER_H */
