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

#include "ti_kv.h"
#include "ti_array.h"
#include "ti_object.h"

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
		return json_name( "no_name" );
	}

	std::string ti_kv::json_name( daw::string_view member_name ) const {
		std::string result =
		  "json_key_value<" + member_name + ", " + name( ) + ", ";
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
