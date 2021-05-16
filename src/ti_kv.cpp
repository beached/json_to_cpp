// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, version 1.0. (see accompanying
// file license or copy at http://www.boost.org/license_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#include "types/ti_kv.h"
#include "types/ti_array.h"
#include "types/ti_base.h"
#include "types/ti_object.h"

#include <daw/daw_visit.h>

namespace daw::json_to_cpp::types {
	ti_kv::ti_kv( std::string obj_name )
	  : value( std::make_unique<child_t>( ) )
	  , kv_name( obj_name ) {}

	ti_kv::ti_kv( ti_kv const &other )
	  : is_optional( other.is_optional )
	  , value( std::make_unique<child_t>( *other.value ) )
	  , kv_name( other.kv_name ) {}

	ti_kv &ti_kv::operator=( ti_kv const &rhs ) {
		if( this != &rhs ) {
			is_optional = rhs.is_optional;
			*value = *rhs.value;
			kv_name = rhs.kv_name;
		}
		return *this;
	}

	std::string ti_kv::name( ) const {
		if( !value ) {
			return "std::unordered_map<std::string, " + ti_null::name( ) + ">";
		}
		return "std::unordered_map<std::string, " +
		       daw::visit_nt(
		         value->front( ).second,
		         []( auto const &item ) -> std::string { return item.name( ); },
		         []( auto const *item ) -> std::string { return item->name( ); } ) +
		       ">";
	}

	std::string ti_kv::array_member_info( ) const {
		return json_name( "no_name", false, "" );
	}

	std::string ti_kv::json_name( daw::string_view member_name, bool use_cpp20,
	                              daw::string_view parent_name ) const {
		std::string result =
		  "json_key_value<" +
		  impl::format_member_name( member_name, use_cpp20, parent_name ) + ", " +
		  name( ) + ", ";
		if( value ) {
			result += daw::visit_nt(
			  value->front( ).second,
			  []( auto const &item ) -> std::string {
				  return item.array_member_info( );
			  },
			  []( auto const *item ) -> std::string {
				  return item->array_member_info( );
			  } );
		} else {
			result += ti_null::array_member_info( );
		}
		result += ">";
		return result;
	}

} // namespace daw::json_to_cpp::types
