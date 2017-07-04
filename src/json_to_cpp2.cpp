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

#include <daw/daw_poly_vector.h>

#include "daw_json_parser_v2_state.h"
#include "ti_value.h"

namespace daw {
	namespace json {
		struct parse_state_parsers {
			state_in_object_name_t in_object_name;
			state_in_object_value_t in_object_value;
			state_in_array_t in_array;
			state_none_t none;

			void in_object_name_on_string( daw::json_to_cpp::types::ti_object &current_obj, boost::string_view value ) {
				// Store member name on stack
				current_obj.member_name_stack = value.to_string( );
				// Set state to in_object_value
			}

			void in_object_name_on_object_end( ) {
				// Set current value to parent
				//
			}

			void in_object_value_on_object_begin( ) {}

			parse_state_parsers( )
			    :

			    struct parse_state_t {
				daw::daw_poly_vector_t<daw::json::state::state_t> state_stack;
				daw::json_to_cpp::types::ti_value result;
				daw::json_to_cpp::types::ti_value *current_value;

				void on_object_begin( );
				void on_object_end( );
				void on_array_begin( );
				void on_array_end( );
				void on_string( boost::string_view );
				void on_integral( boost::string_view );
				void on_real( boost::string_view );
				void on_boolean( bool );
				void on_null( );
			}; // parse_state_t
		}      // namespace json
	}          // namespace json

