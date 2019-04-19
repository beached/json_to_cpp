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

#include <string>
#include <utility>

#include <daw/daw_visit.h>

#include "ti_value.h"

namespace daw::json_to_cpp::types {
	std::string ti_value::name( ) const noexcept {
		return daw::visit_nt( value,
		                      []( auto const &item ) { return item.name( ); } );
	}

	std::string ti_value::json_name( std::string member_name ) const noexcept {
		return daw::visit_nt( value, [&member_name]( auto const &item ) {
			return item.json_name( member_name );
		} );
	}

	std::string ti_value::array_member_info( ) const {
		return daw::visit_nt(
		  value, []( auto const &item ) { return item.array_member_info( ); } );
	}

	daw::ordered_map<std::string, ti_types_t> const &ti_value::children( ) const {
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

	daw::ordered_map<std::string, ti_types_t> &ti_value::children( ) {
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
} // namespace daw::json_to_cpp::types
