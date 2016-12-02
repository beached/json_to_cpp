
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

#include <boost/utility/string_view.hpp>
#include <exception>
#include <string>
#include <vector>

#include <daw/daw_exception.h>
#include <daw/daw_variant.h>

#include "daw_json_parser_v2_state.h"
#include "ti_value.h"

namespace daw {
	namespace json {
		namespace state {
			state_t::~state_t( ) { }

			void state_t::on_object_begin( ) { 
				throw std::runtime_error( this->to_string( ) + ": Unexpected state change: on_object_begin" ); 
			}

			void state_t::on_object_end( ) { 
				throw std::runtime_error( this->to_string( ) + ": Unexpected state change: on_object_end" ); 
			}

			void state_t::on_array_begin( ) { 
				throw std::runtime_error( this->to_string( ) + ": Unexpected state change: on_array_begin" );
			}

			void state_t::on_array_end( ) { 
				throw std::runtime_error( this->to_string( ) + ": Unexpected state change: on_array_end" );
			}

			void state_t::on_string( boost::string_view ) { 
				throw std::runtime_error( this->to_string( ) + ": Unexpected state change: on_string" );
			}

			void state_t::on_integral( boost::string_view ) { 
				throw std::runtime_error( this->to_string( ) + ": Unexpected state change: on_integral" );
			}

			void state_t::on_real( boost::string_view ) { 
				throw std::runtime_error( this->to_string( ) + ": Unexpected state change: on_real" );
			}

			void state_t::on_boolean( bool ) { 
				throw std::runtime_error( this->to_string( ) + ": Unexpected state change: on_boolean" );
			}

			void state_t::on_null( ) { 
				throw std::runtime_error( this->to_string( ) + ": Unexpected state change: on_null" );
			}
		}	// namespace state
	}	// namespace json
}	// namespace daw

