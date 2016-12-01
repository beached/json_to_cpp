
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

			std::vector<state_t*> & state_stack( ) {
				static std::vector<state_t*> result = { get_state_fn( current_state_t::none ) };
				return result;
			}

			state_t & current_state( ) {
				return *state_stack( ).back( );
			}

			void push_and_set_next_state( current_state_t s ) {
				state_stack( ).push_back( get_state_fn( s ) );
			}

			void set_next_state( current_state_t s ) {
				state_stack( ).back( ) = get_state_fn( s );
			}

			void pop_state( ) {
				state_stack( ).pop_back( );
			}

			void pop_value( ) {
				//value_stack( ).pop_back( );
			}

			//
			// state_in_object_name
			//

			state_in_object_name_t::~state_in_object_name_t( ) { }

			std::string state_in_object_name_t::to_string( ) const {
				return "state_in_object_name";
			}

			void state_in_object_name_t::on_string( boost::string_view value ) {
				//value_t name{ value };		

				// Set current object name value_t
				set_next_state( current_state_t::in_object_value );
			}

			void state_in_object_name_t::on_object_end( ) {
				// Save value_t
				// Assumes state is not empty	
				pop_state( );
			}

			state_in_object_value_t::~state_in_object_value_t( ) { }

			std::string state_in_object_value_t::to_string( ) const {
				return "state_in_object_value";
			}

			void state_in_object_value_t::on_object_begin( ) {
				// Save data
				//push_and_set_next_value( value_t{ } );
				set_next_state( current_state_t::in_object_name );
				push_and_set_next_state( current_state_t::in_object_name );
			}

			void state_in_object_value_t::on_array_begin( ) {
				//push_and_set_next_value( value_t{ } );
				set_next_state( current_state_t::in_object_name );
				push_and_set_next_state( current_state_t::in_array );
			}

			void state_in_object_value_t::on_null( ) {
				// Value is already null
				set_next_state( current_state_t::in_object_name );
			}

			//
			// state_in_object_value
			//

			void state_in_object_value_t::on_integral( boost::string_view value ) {
				// Save data
				//current_value( ) = value_t{ to_integral( value ) };
				set_next_state( current_state_t::in_object_name );
			}

			void state_in_object_value_t::on_real( boost::string_view value ) {
				// Save data
				//current_value( ) = value_t{ to_real( value ) };
				set_next_state( current_state_t::in_object_name );
			}

			void state_in_object_value_t::on_string( boost::string_view value ) {
				// Save data
				//current_value( ) = value_t{ value.to_string( ) };
				set_next_state( current_state_t::in_object_name );
			}

			void state_in_object_value_t::on_boolean( bool value ) {
				// Save data
				//current_value( ) = value_t{ value };
				set_next_state( current_state_t::in_object_name );
			}

			//
			// state_in_array_t
			//

			state_in_array_t::~state_in_array_t( ) { }

			std::string state_in_array_t::to_string( ) const {
				return "state_in_array";
			}

			void state_in_array_t::on_object_begin( )  {
				// Save data
				push_and_set_next_state( current_state_t::in_object_name );
			}

			void state_in_array_t::on_array_begin( )  {
				// Save data
				push_and_set_next_state( current_state_t::in_array );
			}

			void state_in_array_t::on_array_end( )  {
				// Save data
				pop_state( );
			}
			void state_in_array_t::on_null( )  {
				// Save data
			}

			void state_in_array_t::on_integral( boost::string_view value )  {
				// Save data
			}

			void state_in_array_t::on_real( boost::string_view value )  {
				// Save data
			}

			void state_in_array_t::on_string( boost::string_view value )  {
				// Save data
			}

			void state_in_array_t::on_boolean( bool value )  {
				// Save data
			}

			//
			// state_none_t
			//

			state_none_t::~state_none_t( ) { }

			std::string state_none_t::to_string( ) const {
				return "state_none";
			}

			void state_none_t::on_object_begin( ) {
				// Save data
				push_and_set_next_state( current_state_t::in_object_name );
			}

			void state_none_t::on_array_begin( ) {
				// Save data
				push_and_set_next_state( current_state_t::in_array );
			}

			void state_none_t::on_null( ) {
				// Save data
			}

			void state_none_t::on_integral( boost::string_view value ) {
				// Save data
			}

			void state_none_t::on_real( boost::string_view value ) {
				// Save data
			}

			void state_none_t::on_string( boost::string_view value ) {
				// Save data
			}

			void state_none_t::on_boolean( bool value ) {
				// Save data
			}

			state_t * get_state_fn( current_state_t s ) noexcept {
				static state_none_t s_none{ };
				static state_in_object_name_t s_in_object_name{ };
				static state_in_object_value_t s_in_object_value{ };
				static state_in_array_t s_in_array{ };
				switch( s ) {
					case current_state_t::none:  
						return &s_none;
					case current_state_t::in_object_name:
						return &s_in_object_name;
					case current_state_t::in_object_value:
						return &s_in_object_value;
					case current_state_t::in_array:
						return &s_in_array;
					case current_state_t::current_state_t_size:
					default:
						std::cerr << "Unknown state" << std::endl;
						std::abort( );
				}
			}

			state_t const & state_control_t::current_state( ) const {
				return daw::json::state::current_state( );	
			}

			void state_control_t::clear_buffer( ) { 
				buffer.clear( ); 
			}
			
			void state_control_t::push( char c ) { 
				buffer.push_back( c ); 
			} 
			
			void state_control_t::on_object_begin( ) const {
				daw::json::state::current_state( ).on_object_begin( ); 
			}
			
			void state_control_t::on_object_end( ) const {
				daw::json::state::current_state( ).on_object_end( ); 
			}
			
			void state_control_t::on_array_begin( ) const {
				daw::json::state::current_state( ).on_array_begin( ); 
			}
			
			void state_control_t::on_array_end( ) const {
				daw::json::state::current_state( ).on_array_end( ); 
			}
			
			void state_control_t::on_string( boost::string_view value ) const {
				daw::json::state::current_state( ).on_string( value ); 
			}
			
			void state_control_t::on_integral( boost::string_view value ) const {
				daw::json::state::current_state( ).on_integral( value ); 
			}
			
			void state_control_t::on_real( boost::string_view value ) const {
				daw::json::state::current_state( ).on_real( value ); 
			}
			
			void state_control_t::on_boolean( bool value ) const {
				daw::json::state::current_state( ).on_boolean( value ); 
			}
			
			void state_control_t::on_null( ) const {
				daw::json::state::current_state( ).on_null( ); 
			}
		}	// namespace state
	}	// namespace json
}	// namespace daw


