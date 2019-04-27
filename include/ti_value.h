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
#include <optional>
#include <string>

#include <daw/cpp_17.h>
#include <daw/daw_enable_if.h>
#include <daw/daw_ordered_map.h>
#include <daw/daw_poly_var.h>
#include <daw/daw_visit.h>

#include "ti_types.h"

namespace daw::json_to_cpp::types {
	struct ti_value {
		ti_types_t value;

		template<
		  typename Derived,
		  daw::enable_if_t<std::is_constructible_v<ti_types_t, Derived>> = nullptr>
		explicit constexpr ti_value( Derived &&other )
		  : value( std::forward<Derived>( other ) ) {}

		template<
		  typename Derived,
		  daw::enable_if_t<std::is_constructible_v<ti_types_t, Derived>> = nullptr>
		constexpr ti_value &operator=( Derived &&rhs ) {
			value = std::forward<Derived>( rhs );
			return *this;
		}

		inline std::string name( ) const noexcept {
			return daw::visit_nt( value,
			                      []( auto const &item ) { return item.name( ); } );
		}

		inline std::string json_name( std::string member_name ) const noexcept {
			return daw::visit_nt( value, [&member_name]( auto const &item ) {
				return item.json_name( member_name );
			} );
		}

		inline std::string array_member_info( ) const {
			return daw::visit_nt(
			  value, []( auto const &item ) { return item.array_member_info( ); } );
		}

		inline daw::ordered_map<std::string, ti_types_t> const &children( ) const {
			return daw::visit_nt(
			  value,
			  []( ti_array const &v )
			    -> daw::ordered_map<std::string, ti_types_t> const & {
				  return *v.children;
			  },
			  []( ti_object const &v )
			    -> daw::ordered_map<std::string, ti_types_t> const & {
				  return *v.children;
			  },
			  []( ... ) -> daw::ordered_map<std::string, ti_types_t> const & {
				  std::terminate( );
			  } );
		}

		inline daw::ordered_map<std::string, ti_types_t> &children( ) {
			return daw::visit_nt(
			  value,
			  []( ti_array &v ) -> daw::ordered_map<std::string, ti_types_t> & {
				  return *v.children;
			  },
			  []( ti_object &v ) -> daw::ordered_map<std::string, ti_types_t> & {
				  return *v.children;
			  },
			  []( ... ) -> daw::ordered_map<std::string, ti_types_t> & {
				  std::terminate( );
			  } );
		}
		template<typename Function>
		constexpr void on_children( Function ) const {}

		constexpr bool &is_optional( ) noexcept {
			return daw::visit_nt(
			  value, []( auto &item ) -> bool & { return item.is_optional; },
			  []( auto *item ) -> bool & { return item->is_optional; } );
		}

		constexpr bool is_optional( ) const noexcept {
			return daw::visit_nt(
			  value, []( auto const &item ) { return item.is_optional; },
			  []( auto const *item ) { return item->is_optional; } );
		}

		constexpr size_t type( ) const noexcept {
			return daw::visit_nt(
			  value, []( auto const &item ) { return item.type; },
			  []( auto const *item ) { return item->type; } );
		}

		constexpr bool is_null( ) const noexcept {
			return daw::visit_nt(
			  value, []( auto const &item ) { return item.is_null; },
			  []( auto const *item ) { return item->is_null; } );
		}
	};
} // namespace daw::json_to_cpp::types
