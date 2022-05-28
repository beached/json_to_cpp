// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, version 1.0. (see accompanying
// file license or copy at http://www.boost.org/license_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include <cstddef>
#include <type_traits>
#include <variant>

#include <daw/cpp_17.h>
#include <daw/daw_string_view.h>

namespace daw::json_to_cpp::types::impl {
	inline constexpr size_t ti_null_pos = 0;
	inline constexpr size_t ti_array_pos = 1;
	inline constexpr size_t ti_boolean_pos = 2;
	inline constexpr size_t ti_integral_pos = 3;
	inline constexpr size_t ti_object_pos = 4;
	inline constexpr size_t ti_real_pos = 5;
	inline constexpr size_t ti_string_pos = 6;
	inline constexpr size_t ti_kv_pos = 7;

	template<typename Variant, typename T, size_t Pos>
	inline constexpr bool validate_pos_value_v =
	  std::is_same_v<daw::remove_cvref_t<decltype( std::get<Pos>( std::declval<Variant>( ) ) )>, T>;

	template<typename... Ts>
	struct pack_list {};

	template<template<class...> class Variant, typename... Args>
	auto var_to_pack_impl( Variant<Args...> ) -> pack_list<Args...>;

	template<typename Variant>
	using var_to_pack_t = decltype( var_to_pack_impl( std::declval<Variant>( ) ) );

	template<typename... Args0, typename... Args1>
	constexpr bool are_same( pack_list<Args0...>, pack_list<Args1...> ) noexcept {

		if( sizeof...( Args0 ) != sizeof...( Args1 ) ) {
			return false;
		}
		return ( std::is_same_v<Args0, Args1> and ... );
	}

	inline std::string
	format_member_name( daw::string_view name, bool use_cpp20, daw::string_view parent_name ) {
		if( use_cpp20 ) {
			std::string result = "\"";
			result.reserve( name.size( ) + 2 );
			result.append( std::data( name ), daw::data_end( name ) );
			result += "\"";
			return result;
		}
		if( parent_name.empty( ) ) {
			return static_cast<std::string>( name );
		}
		std::string result = "mem_";
		result.reserve( name.size( ) + 4 );
		result.append( std::data( name ), daw::data_end( name ) );
		return result;
	}
} // namespace daw::json_to_cpp::types::impl
