// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, version 1.0. (see accompanying
// file license or copy at http://www.boost.org/license_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#include <cstddef>
#include <string>

#include <daw/daw_visit.h>
#include <daw/json/daw_json_value_t.h>

#include "types/ti_array.h"
#include "types/ti_base.h"
#include "types/ti_types.h"

namespace daw::json_to_cpp::types {
	namespace {
		struct name_visitor {
			template<typename Item>
			constexpr decltype( auto ) operator( )( Item &&i ) const noexcept {
				return i.name( );
			}
		};

		template<typename Variant>
		decltype( auto ) name( Variant &&v ) noexcept {
			return daw::visit_nt( std::forward<Variant>( v ), name_visitor{ } );
		}

		struct array_member_info_visitor {
			template<typename Item>
			constexpr decltype( auto ) operator( )( Item &&i ) const noexcept {
				return i.array_member_info( );
			}
		};
		template<typename Variant>
		inline decltype( auto ) array_member_info( Variant &&v ) noexcept {
			return daw::visit_nt( std::forward<Variant>( v ),
			                      array_member_info_visitor{ } );
		}
	} // namespace

	std::string ti_array::name( ) const {
		if( children->empty( ) ) {
			return "std::vector<" + ti_null::name( ) + ">";
		}
		return "std::vector<" +
		       daw::json_to_cpp::types::name( children->front( ).second ) + ">";
	}

	std::string ti_array::json_name( daw::string_view member_name, bool use_cpp20,
	                                 daw::string_view parent_name ) const {
		if( children->empty( ) ) {
			return "json_array<" +
			       impl::format_member_name( member_name, use_cpp20, parent_name ) +
			       ", " + ti_null::array_member_info( ) + ", " + name( ) + ">";
		}
		return "json_array<" +
		       impl::format_member_name( member_name, use_cpp20, parent_name ) +
		       ", " +
		       daw::json_to_cpp::types::array_member_info(
		         children->front( ).second ) +
		       ", " + name( ) + ">";
	}

	std::string ti_array::array_member_info( ) const {
		return json_name( "no_name", false, "" );
	}

	ti_array::ti_array( )
	  : children( std::make_unique<child_t>( ) ) {}

	ti_array::ti_array( const ti_array &other )
	  : children( std::make_unique<child_t>( *other.children ) )
	  , is_optional( other.is_optional ) {}

	ti_array &ti_array::operator=( ti_array const &rhs ) {
		if( this != &rhs ) {
			*children = *rhs.children;
			is_optional = rhs.is_optional;
		}
		return *this;
	}

} // namespace daw::json_to_cpp::types
