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
