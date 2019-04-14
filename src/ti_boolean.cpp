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

#include "ti_boolean.h"

namespace daw::json_to_cpp::types {
	size_t ti_boolean::type( ) const {
		return daw::json::json_value_t::index_of<
		  daw::json::json_value_t::boolean_t>( );
	}

	std::string ti_boolean::name( ) const {
		return "bool";
	}

	std::string ti_boolean::array_member_info( ) const {
		return "json_bool<no_name>";
	}

	std::string ti_boolean::json_name( std::string member_name ) const {
		return "json_bool<" + member_name + ">";
	}

	type_info_t *ti_boolean::clone( ) const {
		return new ti_boolean( *this );
	}
} // namespace daw::json_to_cpp::types
