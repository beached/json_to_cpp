// The MIT License (MIT)
//
// Copyright (c) 2016-2017 Darrell Wright
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files( the "Software" ), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include <algorithm>
#include <map>
#include <string>

#include "ti_value.h"

namespace daw {
	namespace json_to_cpp {
		namespace types {
			ti_value::ti_value( ) : value{nullptr} {}
			ti_value::ti_value( ti_value &&other ) : value{std::exchange( other.value, nullptr )} {}

			ti_value &ti_value::operator=( ti_value const &rhs ) {
				if( this != &rhs ) {
					ti_value tmp{rhs};
					using std::swap;
					swap( *this, tmp );
				}
				return *this;
			}

			ti_value &ti_value::operator=( ti_value &&rhs ) {
				if( this != &rhs ) {
					value = std::exchange( rhs.value, nullptr );
				}
				return *this;
			}

			type_info_t::type_info_t( ) : is_optional{false}, children{} {}

			type_info_t::~type_info_t( ) {}

			std::string ti_value::name( ) const noexcept {
				return value->name( );
			}

			daw::json::impl::value_t::value_types ti_value::type( ) const {
				return value->type( );
			}
			std::map<std::string, ti_value> const &ti_value::children( ) const {
				return value->children;
			}

			std::map<std::string, ti_value> &ti_value::children( ) {
				return value->children;
			}

			bool &ti_value::is_optional( ) noexcept {
				return value->is_optional;
			}

			bool const &ti_value::is_optional( ) const noexcept {
				return value->is_optional;
			}

			daw::json::impl::value_t::value_types ti_null::type( ) const {
				return daw::json::impl::value_t::value_types::null;
			}

			std::string ti_null::name( ) const {
				return "void*";
			}

			ti_null::ti_null( ) : type_info_t{} {
				is_optional = true;
			}

			type_info_t *ti_null::clone( ) const {
				return new ti_null( *this );
			}

			ti_value::ti_value( ti_value const &other ) : value{other.value->clone( )} {}

			ti_value::~ti_value( ) {
				if( nullptr != value ) {
					auto tmp = value;
					value = nullptr;
					delete tmp;
				}
			}

			daw::json::impl::value_t::value_types ti_integral::type( ) const {
				return daw::json::impl::value_t::value_types::integral;
			}

			std::string ti_integral::name( ) const {
				return "int64_t";
			}

			type_info_t *ti_integral::clone( ) const {
				return new ti_integral( *this );
			}

			daw::json::impl::value_t::value_types ti_real::type( ) const {
				return daw::json::impl::value_t::value_types::real;
			}

			std::string ti_real::name( ) const {
				return "double";
			}

			type_info_t *ti_real::clone( ) const {
				return new ti_real( *this );
			}

			daw::json::impl::value_t::value_types ti_boolean::type( ) const {
				return daw::json::impl::value_t::value_types::boolean;
			}

			std::string ti_boolean::name( ) const {
				return "bool";
			}

			type_info_t *ti_boolean::clone( ) const {
				return new ti_boolean( *this );
			}

			daw::json::impl::value_t::value_types ti_string::type( ) const {
				return daw::json::impl::value_t::value_types::string;
			}

			std::string ti_string::name( ) const {
				return "std::string";
			}

			type_info_t *ti_string::clone( ) const {
				return new ti_string( *this );
			}

			daw::json::impl::value_t::value_types ti_object::type( ) const {
				return daw::json::impl::value_t::value_types::object;
			}

			std::string ti_object::name( ) const {
				return object_name;
			}

			ti_object::ti_object( std::string obj_name, type_info_t *Parent )
			    : type_info_t{}, object_name{std::move( obj_name )}, parent{Parent} {}

			type_info_t *ti_object::clone( ) const {
				return new ti_object( *this );
			}

			daw::json::impl::value_t::value_types ti_array::type( ) const {
				return daw::json::impl::value_t::value_types::array;
			}

			ti_array::ti_array( type_info_t *Parent ) : type_info_t{}, parent{Parent} {}

			std::string ti_array::name( ) const {
				return "std::vector<" + children.begin( )->second.name( ) + ">";
			}

			type_info_t *ti_array::clone( ) const {
				return new ti_array( *this );
			}

		} // namespace types
	}     // namespace json_to_cpp
} // namespace daw

