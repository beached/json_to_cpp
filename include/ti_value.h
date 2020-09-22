// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, version 1.0. (see accompanying
// file license or copy at http://www.boost.org/license_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include <cstddef>
#include <optional>
#include <string>

#include <daw/cpp_17.h>
#include <daw/daw_enable_if.h>
#include <daw/daw_ordered_map.h>
#include <daw/daw_poly_var.h>
#include <daw/daw_visit.h>

#include "types/ti_types.h"

namespace daw::json_to_cpp::types {
	struct ti_value {
		ti_types_t value;

		template<typename Derived,
		         daw::enable_when_t<std::is_constructible_v<ti_types_t, Derived>> =
		           nullptr>
		explicit constexpr ti_value( Derived &&other )
		  : value( std::forward<Derived>( other ) ) {}

		template<typename Derived,
		         daw::enable_when_t<std::is_constructible_v<ti_types_t, Derived>> =
		           nullptr>
		constexpr ti_value &operator=( Derived &&rhs ) {
			value = std::forward<Derived>( rhs );
			return *this;
		}

		inline std::string name( ) const noexcept {
			return daw::visit_nt( value,
			                      []( auto const &item ) { return item.name( ); } );
		}

		inline std::string
		json_name( std::string member_name, bool use_cpp20,
		           daw::string_view parent_name ) const noexcept {
			return daw::visit_nt(
			  value, [&member_name, use_cpp20, parent_name]( auto const &item ) {
				  return item.json_name( member_name, use_cpp20, parent_name );
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
			  []( auto ) -> daw::ordered_map<std::string, ti_types_t> const & {
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
			  []( auto ) -> daw::ordered_map<std::string, ti_types_t> & {
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
