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
#include <variant>

#include <daw/daw_ordered_map.h>
#include <daw/daw_string_view.h>

#include "ti_boolean.h"
#include "ti_integral.h"
#include "ti_null.h"
#include "ti_real.h"
#include "ti_string.h"

namespace daw::json_to_cpp::types {
	struct ti_array;
	struct ti_object;
	struct ti_array {
		using child_t =
		  daw::ordered_map<std::string,
		                   std::variant<ti_null, ti_array, ti_boolean, ti_integral,
		                                ti_object, ti_real, ti_string>>;

		std::unique_ptr<child_t> children;

		bool is_optional = false;
		static constexpr bool is_null = false;

		ti_array( );
		ti_array( ti_array && ) noexcept = default;
		ti_array &operator=( ti_array && ) noexcept = default;
		~ti_array( ) = default;
		ti_array( ti_array const &other );
		ti_array &operator=( ti_array const &rhs );

		static constexpr size_t type( ) noexcept {
			return daw::json::json_value_t::index_of<
			  daw::json::json_value_t::array_t>( );
		}

		std::string name( ) const;
		std::string json_name( std::string member_name ) const;
		std::string array_member_info( ) const;
	};
} // namespace daw::json_to_cpp::types
