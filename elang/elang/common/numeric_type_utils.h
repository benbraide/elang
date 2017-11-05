#pragma once

#ifndef ELANG_NUMERIC_TYPE_UTILS_H
#define ELANG_NUMERIC_TYPE_UTILS_H

namespace elang::common{
	enum class numeric_type_id{
		int8,
		uint8,
		int16,
		uint16,
		int32,
		uint32,
		int64,
		uint64,
		float_,
		double_,
		long_double_,
	};

	template <class type>
	struct numeric_type_to_id;

	template <>
	struct numeric_type_to_id<__int8>{
		static const numeric_type_id value = numeric_type_id::int8;
	};

	template <>
	struct numeric_type_to_id<unsigned __int8>{
		static const numeric_type_id value = numeric_type_id::uint8;
	};

	template <>
	struct numeric_type_to_id<__int16>{
		static const numeric_type_id value = numeric_type_id::int16;
	};

	template <>
	struct numeric_type_to_id<unsigned __int16>{
		static const numeric_type_id value = numeric_type_id::uint16;
	};

	template <>
	struct numeric_type_to_id<__int32>{
		static const numeric_type_id value = numeric_type_id::int32;
	};

	template <>
	struct numeric_type_to_id<unsigned __int32>{
		static const numeric_type_id value = numeric_type_id::uint32;
	};

	template <>
	struct numeric_type_to_id<__int64>{
		static const numeric_type_id value = numeric_type_id::int64;
	};

	template <>
	struct numeric_type_to_id<unsigned __int64>{
		static const numeric_type_id value = numeric_type_id::uint64;
	};

	template <>
	struct numeric_type_to_id<float>{
		static const numeric_type_id value = numeric_type_id::float_;
	};

	template <>
	struct numeric_type_to_id<double>{
		static const numeric_type_id value = numeric_type_id::double_;
	};

	template <>
	struct numeric_type_to_id<long double>{
		static const numeric_type_id value = numeric_type_id::long_double_;
	};

	template <numeric_type_id id>
	struct numeric_id_to_type;

	template <>
	struct numeric_id_to_type<numeric_type_id::int8>{
		typedef __int8 type;
	};

	template <>
	struct numeric_id_to_type<numeric_type_id::uint8>{
		typedef unsigned __int8 type;
	};

	template <>
	struct numeric_id_to_type<numeric_type_id::int16>{
		typedef __int16 type;
	};

	template <>
	struct numeric_id_to_type<numeric_type_id::uint16>{
		typedef unsigned __int16 type;
	};

	template <>
	struct numeric_id_to_type<numeric_type_id::int32>{
		typedef __int32 type;
	};

	template <>
	struct numeric_id_to_type<numeric_type_id::uint32>{
		typedef unsigned __int32 type;
	};

	template <>
	struct numeric_id_to_type<numeric_type_id::int64>{
		typedef __int64 type;
	};

	template <>
	struct numeric_id_to_type<numeric_type_id::uint64>{
		typedef unsigned __int64 type;
	};

	template <>
	struct numeric_id_to_type<numeric_type_id::float_>{
		typedef float type;
	};

	template <>
	struct numeric_id_to_type<numeric_type_id::double_>{
		typedef double type;
	};

	template <>
	struct numeric_id_to_type<numeric_type_id::long_double_>{
		typedef long double type;
	};
}

#endif /* !ELANG_NUMERIC_TYPE_UTILS_H */
