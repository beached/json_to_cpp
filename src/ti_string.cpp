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

#include "ti_string.h"

namespace daw::json_to_cpp::types {
	ti_string::ti_string( bool use_string_view )
	  : m_use_string_view( use_string_view ) {}

	size_t ti_string::type( ) const {
		return daw::json::json_value_t::index_of<
		  daw::json::json_value_t::string_t>( );
	}

	std::string ti_string::name( ) const {
		if( m_use_string_view ) {
			return "std::string_view";
		}
		return "std::string";
	}

	std::string ti_string::array_member_info( ) const {
		if( m_use_string_view ) {
			return "json_string<no_name, std::string_view>";
		}
		return "json_string<no_name>";
	}

	std::string ti_string::json_name( std::string member_name ) const {
		if( m_use_string_view ) {
			return "json_string<" + member_name + ", std::string_view>";
		}
		return "json_string<" + member_name + ">";
	}

	type_info_t *ti_string::clone( ) const {
		return new ti_string( *this );
	}
} // namespace daw::json_to_cpp::types
