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
#include <string>

#include <daw/daw_ordered_map.h>
#include <daw/json/daw_json_value_t.h>

namespace daw::json_to_cpp::types {
	struct type_info_t;

	struct ti_value {
		type_info_t *value = nullptr;

		constexpr ti_value( ) noexcept = default;
		~ti_value( );
		ti_value( ti_value const &other );

		inline ti_value( ti_value &&other ) noexcept
		  : value{std::exchange( other.value, nullptr )} {}

		inline ti_value &operator=( ti_value const &rhs ) {
			if( this != &rhs ) {
				ti_value tmp{rhs};
				using std::swap;
				swap( *this, tmp );
			}
			return *this;
		}

		inline ti_value &operator=( ti_value &&rhs ) {
			if( this != &rhs ) {
				value = std::exchange( rhs.value, nullptr );
			}
			return *this;
		}

		template<typename Derived>
		ti_value( Derived other )
		  : value{new Derived( std::move( other ) )} {}

		template<typename Derived>
		ti_value &operator=( Derived rhs ) {
			value = new Derived( std::move( rhs ) );
			return *this;
		}

		std::string name( ) const noexcept;
		std::string json_name( std::string member_name ) const noexcept;
		std::string array_member_info( ) const;

		daw::ordered_map<std::string, ti_value> const &children( ) const;
		daw::ordered_map<std::string, ti_value> &children( );

		bool &is_optional( ) noexcept;
		bool const &is_optional( ) const noexcept;

		size_t type( ) const;

		inline bool is_null( ) const {
			return type( ) ==
			       daw::json::json_value_t::index_of<json::json_value_t::null_t>( );
		}
	};

	template<typename Derived, typename... Args>
	ti_value create_ti_value( Args &&... args ) {
		ti_value result{};
		auto tmp = new Derived( std::forward<Args>( args )... );
		result.value = std::exchange( tmp, nullptr );
		return result;
	}
} // namespace daw::json_to_cpp::types
