// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, version 1.0. (see accompanying
// file license or copy at http://www.boost.org/license_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#include <cstddef>
#include <string>

#include <daw/json/daw_json_value_t.h>

#include "types/ti_base.h"
#include "types/ti_object.h"
#include "types/ti_types.h"

namespace daw::json_to_cpp::types {

	std::string ti_object::name( ) const {
		return object_name;
	}

	ti_object::ti_object( std::string obj_name )
	  : children( std::make_unique<child_t>( ) )
	  , object_name( std::move( obj_name ) ) {}

	std::string ti_object::json_name( daw::string_view member_name,
	                                  bool use_cpp20,
	                                  daw::string_view parent_name ) const {
		return "json_class<" +
		       impl::format_member_name( member_name, use_cpp20, parent_name ) +
		       ", " + name( ) + ">";
	}

	std::string ti_object::array_member_info( ) const {
		return json_name( "no_name", false, "" );
	}

	ti_object::ti_object( ti_object const &other )
	  : children( std::make_unique<child_t>( *other.children ) )
	  , is_optional( other.is_optional )
	  , object_name( other.object_name ) {}

	ti_object &ti_object::operator=( ti_object const &rhs ) {
		if( this != &rhs ) {
			*children = *rhs.children;
			is_optional = rhs.is_optional;
			object_name = rhs.object_name;
		}
		return *this;
	}

} // namespace daw::json_to_cpp::types
