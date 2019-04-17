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

#include <daw/daw_string_view.h>

namespace daw::json_to_cpp::types {
	class ti_string {
		bool is_optional = false;
		bool m_use_string_view;

	public:
		static constexpr bool is_null = false;

		constexpr explicit ti_string( bool use_string_view ) noexcept
		  : m_use_string_view( use_string_view ) {}

		static constexpr size_t type( ) noexcept {
			return daw::json::json_value_t::index_of<
			  daw::json::json_value_t::string_t>( );
		}

		constexpr daw::string_view name( ) const noexcept {
			if( m_use_string_view ) {
				return "std::string_view";
			}
			return "std::string";
		}

		constexpr daw::string_view array_member_info( ) const noexcept {
			if( m_use_string_view ) {
				return "json_string<no_name, std::string_view>";
			}
			return "json_string<no_name>";
		}

		inline std::string json_name( std::string member_name ) const noexcept {
			if( m_use_string_view ) {
				return "json_string<" + member_name + ", std::string_view>";
			}
			return "json_string<" + member_name + ">";
		}
	};
} // namespace daw::json_to_cpp::types
