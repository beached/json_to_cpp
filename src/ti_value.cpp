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

#include "ti_value.h"
#include "type_info.h"

namespace daw::json_to_cpp::types {
	std::string ti_value::name( ) const noexcept {
		return value->name( );
	}

	std::string ti_value::json_name( std::string member_name ) const noexcept {
		return value->json_name( std::move( member_name ) );
	}

	std::string ti_value::array_member_info( ) const {
		return value->array_member_info( );
	}

	size_t ti_value::type( ) const {
		return value->type( );
	}

	daw::ordered_map<std::string, ti_value> const &ti_value::children( ) const {
		return value->children;
	}

	daw::ordered_map<std::string, ti_value> &ti_value::children( ) {
		return value->children;
	}

	bool &ti_value::is_optional( ) noexcept {
		return value->is_optional;
	}

	bool const &ti_value::is_optional( ) const noexcept {
		return value->is_optional;
	}

	ti_value::~ti_value( ) {
		delete std::exchange( value, nullptr );
	}

	ti_value::ti_value( ti_value const &other )
	  : value( other.value->clone( ) ) {}

	ti_value & ti_value::operator =( ti_value && rhs ) noexcept {
		if( this != &rhs ) {
			value = std::exchange( rhs.value, nullptr );
		}
		return *this;
	}

	ti_value & ti_value::operator =( ti_value const & rhs ) {
		if( this != &rhs ) {
			auto tmp = rhs;
			using std::swap;
			swap( *this, tmp );
		}
		return *this;
	}

	ti_value::ti_value( ti_value && other ) noexcept
			: value{std::exchange( other.value, nullptr )} {}

	bool ti_value::is_null( ) const {
		return type( ) ==
					 daw::json::json_value_t::index_of<json::json_value_t::null_t>( );
	}
} // namespace daw::json_to_cpp::types
