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

#include <daw/daw_visit.h>
#include <daw/json/daw_json_value_t.h>

#include "ti_array.h"

namespace daw::json_to_cpp::types {
	namespace {
		template<typename Variant>
		constexpr bool name( Variant &&v ) noexcept {
			return daw::visit_nt( v, []( auto &&item ) { return item.name( ); } );
		}

		template<typename Variant>
		constexpr bool array_member_info( Variant &&v ) noexcept {
			return daw::visit_nt(
			  v, []( auto &&item ) { return item.array_member_info( ); } );
		}
	} // namespace


	std::string ti_array::name( ) const {
		if( children.empty( ) ) {
			return "std::vector<" + ti_null::name( ) + ">";
		}
		return "std::vector<" + name( children.front( ).second ) + ">";
	}

	std::string ti_array::json_name( std::string member_name ) const {
		if( children.empty( ) ) {
			return "json_array<" + member_name + ", " + name( ) + ", " +
			       ti_null::array_member_info( ) + ">";
		}
		return "json_array<" + member_name + ", " + name( ) + ", " +
		       array_member_info( children.front( ).second ) + ">";
	}

	std::string ti_array::array_member_info( ) const {
		if( children.empty( ) ) {
			return "json_array<no_name, " + ti_null::name( ) + ", " +
			       ti_null::array_member_info( ) + ">";
		}
		return "json_array<no_name, " + name( ) + ", " +
		       array_member_info( children.front( ).second ) + ">";
	}
} // namespace daw::json_to_cpp::types
