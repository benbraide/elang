#pragma once

#ifndef ELANG_MACHINE_VALUE_TYPE_ID_H
#define ELANG_MACHINE_VALUE_TYPE_ID_H

#include <string>

namespace elang::vm{
	enum class machine_value_type_id{
		byte,
		word,
		dword,
		qword,
		float_,
		unknown,
	};

	template <class value_type>
	struct machine_value_type{
		static const machine_value_type_id id = machine_value_type_id::unknown;
	};

	template <>
	struct machine_value_type<__int8>{
		static const machine_value_type_id id = machine_value_type_id::byte;
	};

	template <>
	struct machine_value_type<unsigned __int8>{
		static const machine_value_type_id id = machine_value_type_id::byte;
	};

	template <>
	struct machine_value_type<__int16>{
		static const machine_value_type_id id = machine_value_type_id::word;
	};

	template <>
	struct machine_value_type<unsigned __int16>{
		static const machine_value_type_id id = machine_value_type_id::word;
	};

	template <>
	struct machine_value_type<__int32>{
		static const machine_value_type_id id = machine_value_type_id::dword;
	};

	template <>
	struct machine_value_type<unsigned __int32>{
		static const machine_value_type_id id = machine_value_type_id::dword;
	};

	template <>
	struct machine_value_type<__int64>{
		static const machine_value_type_id id = machine_value_type_id::qword;
	};

	template <>
	struct machine_value_type<unsigned __int64>{
		static const machine_value_type_id id = machine_value_type_id::qword;
	};

	template <>
	struct machine_value_type<float>{
		static const machine_value_type_id id = machine_value_type_id::float_;
	};

	template <>
	struct machine_value_type<double>{
		static const machine_value_type_id id = machine_value_type_id::float_;
	};

	template <>
	struct machine_value_type<long double>{
		static const machine_value_type_id id = machine_value_type_id::float_;
	};

	struct machine_value_type_id_utils{
		static std::size_t machine_value_type_id_byte_size(machine_value_type_id id){
			switch (id){
			case machine_value_type_id::byte:
				return 1u;
			case machine_value_type_id::word:
				return 2u;
			case machine_value_type_id::dword:
				return 4u;
			case machine_value_type_id::qword:
			case machine_value_type_id::float_:
				return 8u;
			default:
				break;
			}

			return 0u;
		}

		static std::string machine_value_type_id_to_string(machine_value_type_id id){
			switch (id){
			case machine_value_type_id::byte:
				return "BYTE";
			case machine_value_type_id::word:
				return "WORD";
			case machine_value_type_id::dword:
				return "DWORD";
			case machine_value_type_id::qword:
				return "QWORD";
			case machine_value_type_id::float_:
				return "FLOAT";
			default:
				break;
			}

			return "";
		}
	};
}

#endif /* !ELANG_MACHINE_VALUE_TYPE_ID_H */
