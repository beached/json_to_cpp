// The MIT License (MIT)
//
// Copyright (c) 2019 Darrell Wright
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files( the "Software" ), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and / or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

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
	inline constexpr bool validate_pos_value_v = std::is_same_v<
	  daw::remove_cvref_t<decltype( std::get<Pos>( std::declval<Variant>( ) ) )>,
	  T>;

	template<typename... Ts>
	struct pack_list {};

	template<template<class...> class Variant, typename... Args>
	auto var_to_pack_impl( Variant<Args...> ) -> pack_list<Args...>;

	template<typename Variant>
	using var_to_pack_t =
	  decltype( var_to_pack_impl( std::declval<Variant>( ) ) );

	template<typename... Args0, typename... Args1>
	constexpr bool are_same( pack_list<Args0...>, pack_list<Args1...> ) noexcept {

		if( sizeof...( Args0 ) != sizeof...( Args1 ) ) {
			return false;
		}
		return ( std::is_same_v<Args0, Args1> and ... );
	}

	inline std::string format_member_name( daw::string_view name, bool use_cpp20,
	                                       daw::string_view parent_name ) {
		if( use_cpp20 ) {
			return "\"" + name + "\"";
		}
		if( parent_name.empty( ) ) {
			return name;
		}
		std::string result = "symbols_" + parent_name.to_string( ) + "::" + name;
		return result;
	}
} // namespace daw::json_to_cpp::types::impl
