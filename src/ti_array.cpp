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

#include "ti_array.h"

namespace daw::json_to_cpp::types {
	size_t ti_array::type( ) const {
		if( children.begin( )->second.is_null( ) ) {
			return daw::json::json_value_t::index_of<
			  daw::json::json_value_t::null_t>( );
		}
		return daw::json::json_value_t::index_of<
		  daw::json::json_value_t::array_t>( );
	}

	std::string ti_array::name( ) const {
		return "std::vector<" + children.begin( )->second.name( ) + ">";
	}

	std::string ti_array::json_name( std::string member_name ) const {
		return "json_array<" + member_name + ", " + name( ) + ", " +
		       children.begin( )->second.array_member_info( ) + ">";
	}

	std::string ti_array::array_member_info( ) const {
		return "json_array<no_name, " + name( ) + ", " +
		       children.begin( )->second.array_member_info( ) + ">";
	}

	type_info_t *ti_array::clone( ) const {
		return new ti_array( *this );
	}
} // namespace daw::json_to_cpp::types
