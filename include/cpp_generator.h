// The MIT License (MIT)
//
// Copyright (c) 2016 Darrell Wright
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

#pragma once

#include "ti_value.h"
#include "json_to_cpp.h"

namespace daw {
	namespace json_to_cpp {
		struct object_state_t final {
			bool has_arrays;
			bool has_integrals;
			bool has_optionals;
			bool has_strings;
			object_state_t( );
			~object_state_t( );
			object_state_t( object_state_t const & ) = default;
			object_state_t( object_state_t && ) = default;
			object_state_t & operator=( object_state_t const & ) = default;
			object_state_t & operator=( object_state_t && ) = default;
		};	// object_state_t

		namespace generate {
			void generate_code( std::vector<types::ti_object> const & obj_info, config_t & config, object_state_t const & obj_state );
		}	// namespace generate
	}	// namespace json_to_cpp
}    // namespace daw

