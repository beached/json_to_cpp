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

#include "ti_real.h"

namespace daw::json_to_cpp::types {
	size_t ti_real::type( ) const {
		return daw::json::json_value_t::index_of<
		  daw::json::json_value_t::real_t>( );
	}

	std::string ti_real::name( ) const {
		return "double";
	}

	std::string ti_real::array_member_info( ) const {
		return "json_number<no_name>";
	}

	std::string ti_real::json_name( std::string member_name ) const {
		return "json_number<" + member_name + ">";
	}

	type_info_t *ti_real::clone( ) const {
		return new ti_real( *this );
	}
} // namespace daw::json_to_cpp::types
