#pragma once

#ifndef ELANG_OUTPUT_WRITER_H
#define ELANG_OUTPUT_WRITER_H

#include <mutex>
#include <string>
#include <memory>

namespace elang{
	namespace common{
		enum class output_writer_manip{
			nil,
			begin,
			end,
			flush,
			newline,
		};

		enum class output_writer_error{
			nil,
			cannot_write_narrow,
			cannot_write_wide,
		};

		class output_writer{
		public:
			typedef output_writer_manip manip_type;
			typedef output_writer_error error_type;

			typedef std::mutex lock_type;
			typedef std::shared_ptr<lock_type> lock_ptr_type;

			virtual ~output_writer() = default;

			virtual output_writer &operator <<(manip_type manip){
				switch (manip){
				case manip_type::begin:
					get_lock_().lock();
					break;
				case manip_type::end:
					get_lock_().unlock();
					break;
				default:
					break;
				}

				return *this;
			}

			virtual output_writer &operator <<(__int8 value) = 0;

			virtual output_writer &operator <<(unsigned __int8 value) = 0;

			virtual output_writer &operator <<(__int16 value) = 0;

			virtual output_writer &operator <<(unsigned __int16 value) = 0;

			virtual output_writer &operator <<(__int32 value) = 0;

			virtual output_writer &operator <<(unsigned __int32 value) = 0;

			virtual output_writer &operator <<(__int64 value) = 0;

			virtual output_writer &operator <<(unsigned __int64 value) = 0;

			virtual output_writer &operator <<(float value) = 0;

			virtual output_writer &operator <<(double value) = 0;

			virtual output_writer &operator <<(long double value) = 0;

			virtual output_writer &operator <<(const char *value) = 0;

			virtual output_writer &operator <<(const wchar_t *value) = 0;

			virtual output_writer &operator <<(const std::string &value) = 0;

			virtual output_writer &operator <<(const std::wstring &value) = 0;

		protected:
			virtual void pre_write_(){}

			lock_type &get_lock_(){
				if (lock_ == nullptr)
					lock_ = std::make_shared<std::mutex>();
				return *lock_;
			}

			lock_ptr_type lock_;
		};
	}
}

#endif /* !ELANG_OUTPUT_WRITER_H */
