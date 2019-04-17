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

#pragma once

#include <cstddef>
#include <string>

#include <daw/cpp_17.h>
#include <daw/daw_enable_if.h>
#include <daw/daw_ordered_map.h>
#include <daw/daw_poly_var.h>

#include "ti_array.h"
#include "ti_boolean.h"
#include "ti_integral.h"
#include "ti_null.h"
#include "ti_object.h"
#include "ti_real.h"
#include "ti_string.h"
#include "type_info.h"

namespace daw::json_to_cpp::types {
	class ti_value {
		using ti_value_t =
		  daw::poly_var<type_info_t, ti_array, ti_boolean, ti_integral, ti_null,
		                ti_object, ti_real, ti_string>;

		ti_value_t m_value;

	public:
		template<typename Derived,
		         daw::enable_if_t<!std::is_same_v<
		           ti_value_t, daw::remove_cvref_t<Derived>>> = nullptr>
		explicit ti_value_t( Derived &&other )
		  : value( std::forward<Derived>( other ) ) {}

		template<typename Derived,
		         daw::enable_if_t<!std::is_same_v<
		           ti_value_t, daw::remove_cvref_t<Derived>>> = nullptr>
		ti_value_t &operator=( Derived &&rhs ) {
			value = std::forward<Derived>( rhs );
			return *this;
		}

		std::string name( ) const noexcept;
		std::string json_name( std::string member_name ) const noexcept;
		std::string array_member_info( ) const;

		daw::ordered_map<std::string, ti_value_t> const &children( ) const;
		daw::ordered_map<std::string, ti_value_t> &children( );

		bool &is_optional( ) noexcept;
		bool const &is_optional( ) const noexcept;

		size_t type( ) const;

		bool is_null( ) const;
	};
} // namespace daw::json_to_cpp::types
