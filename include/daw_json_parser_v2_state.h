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

#include <boost/utility/string_view.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/utility/string_view.hpp>
#include <string>
#include <vector>

#include "daw_json_parser_v2.h"

namespace daw {
	namespace json {
		namespace state {
			struct state_t {
				state_t( ) = default;
				state_t( state_t const & ) = default;
				state_t( state_t && ) = default;
				state_t & operator=( state_t const & ) = default;
				state_t & operator=( state_t && ) = default;

				virtual ~state_t( );

				virtual void on_object_begin( );
				virtual void on_object_end( );
				virtual void on_array_begin( );
				virtual void on_array_end( );
				virtual void on_string( boost::string_view );
				virtual void on_integral( boost::string_view );
				virtual void on_real( boost::string_view );
				virtual void on_boolean( bool ); 
				virtual void on_null( );
			};	// state_t

			template<typename CBOnString, typename CBOnObjEnd>
			class state_in_object_name_t final: public state_t {
				CBOnString m_cb_on_string;
				CBOnObjEnd m_cb_on_object_end;
			public:
				state_in_object_name_t( ):
						state_t{ },
						m_cb_on_string{ },
						m_cb_on_object_end{ } { }

				state_in_object_name_t( CBOnString cb_on_string, CBOnObjEnd cb_on_object_end ):
						state_t{ },
						m_cb_on_string{ std::move( cb_on_string ) },
						m_cb_on_object_end{ std::move( cb_on_object_end ) } { }

				state_in_object_name_t( state_in_object_name_t const & ) = default;
				state_in_object_name_t( state_in_object_name_t && ) = default;
				state_in_object_name_t & operator=( state_in_object_name_t const & ) = default;
				state_in_object_name_t & operator=( state_in_object_name_t && ) = default;

				~state_in_object_name_t( );

				void on_string( boost::string_view value ) override {
					m_cb_on_string( value );
				}

				void on_object_end( ) override {
					m_cb_on_object_end( );
				}
			};	// state_in_object_name

			template<typename CBOnString, typename CBOnObjEnd>
			state_in_object_name_t::~state_in_object_name_t( ) { }

			template<typename CBOnString, typename CBOnObjEnd>
			auto make_state_in_object_name_t( CBOnString cb_on_string, CBOnObjEnd cb_on_object_end ) {
				return state_in_object_name_t<CBOnString, CBOnObjEnd>{ std::move( cb_on_string ), std::move( cb_on_object_end ) };
			}

			template<typename CBOnObjBegin, typename CBOnAryBegin, typename CBOnNull, typename CBOnIntegral, typename CBOnReal, typename CBOnString, typename CBOnBoolean>
			struct state_in_object_value_t final: public state_t {
				CBOnObjBegin m_cb_on_object_begin; 
				CBOnAryBegin m_cb_on_array_begin; 
				CBOnNull m_cb_on_null; 
				CBOnIntegral m_cb_on_integral; 
				CBOnReal m_cb_on_real; 
				CBOnString m_cb_on_string; 
				CBOnBoolean m_cb_on_boolean;

				state_in_object_value_t( ):
						state_t{ },
						m_cb_on_object_begin{ },
						m_cb_on_array_begin{ }, 
						m_cb_on_null{ }, 
						m_cb_on_integral{ }, 
						m_cb_on_real{ }, 
						m_cb_on_string{ }, 
						m_cb_on_boolean{ } { }

				state_in_object_value_t( CBOnObjBegin cb_on_object_begin, CBOnAryBegin cb_on_array_begin, CBOnNull cb_on_null, CBOnIntegral cb_on_integral, CBOnReal cb_on_real, CBOnString cb_on_string, CBOnBoolean cb_on_boolean ):
						state_t{ },
						m_cb_on_object_begin{ std::move( cb_on_object_begin ) },
						m_cb_on_array_begin{ std::move( cb_on_array_begin ) }, 
						m_cb_on_null{ std::move( cb_on_null ) }, 
						m_cb_on_integral{ std::move( cb_on_integral ) }, 
						m_cb_on_real{ std::move( cb_on_real ) }, 
						m_cb_on_string{ std::move( cb_on_string ) }, 
						m_cb_on_boolean{ std::move( cb_on_boolean ) } { }

				state_in_object_value_t( state_in_object_value_t const & ) = default;
				state_in_object_value_t( state_in_object_value_t && ) = default;
				state_in_object_value_t & operator=( state_in_object_value_t const & ) = default;
				state_in_object_value_t & operator=( state_in_object_value_t && ) = default;

				~state_in_object_value_t( );

				void on_object_begin( ) override {
					m_cb_on_object_begin( );
				}

				void on_array_begin( ) override {
					m_cb_on_array_begin( );
				}

				void on_null( ) override {
					m_cb_on_null( );
				}

				void on_integral( boost::string_view value ) override {
					m_cb_on_integral( value );
				}

				void on_real( boost::string_view value ) override {
					m_cb_on_real( value );
				}

				void on_string( boost::string_view value ) override {
					m_cb_on_string( value );
				}

				void on_boolean( bool value ) override {
					m_cb_on_boolean( value );
				}
			};	// state_in_object_value_t

			template<typename CBOnObjBegin, typename CBOnAryBegin, typename CBOnNull, typename CBOnIntegral, typename CBOnReal, typename CBOnString, typename CBOnBoolean>
			state_in_object_value::~state_in_object_value_t( ) { }

			template<typename CBOnObjBegin, typename CBOnAryBegin, typename CBOnNull, typename CBOnIntegral, typename CBOnReal, typename CBOnString, typename CBOnBoolean>
			auto make_state_in_object_value_t( CBOnObjBegin cb_on_object_begin, CBOnAryBegin cb_on_array_begin, CBOnNull cb_on_null, CBOnIntegral cb_on_integral, CBOnReal cb_on_real, CBOnString cb_on_string, CBOnBoolean cb_on_boolean ) {
				return state_in_object_value_t<
							CBOnObjBegin, 
							CBOnAryBegin, 
							CBOnNull, 
							CBOnIntegral, 
							CBOnReal, 
							CBOnString, 
							CBOnBoolean>{ std::move( cb_on_object_begin ), 
									std::move( cb_on_array_begin ), 
									std::move( cb_on_null ), 
									std::move( cb_on_integral ),
									std::move( cb_on_real ),
									std::move( cb_on_string ),
									std::move( cb_on_boolean };
			}

			template<typename CBOnObjBegin, typename CBOnAryBegin, typename CBOnAryEnd, typename CBOnNull, typename CBOnIntegral, typename CBOnReal, typename CBOnString, typename CBOnBoolean>
			struct state_in_array_t: public state_t {
				CBOnObjBegin m_cb_on_object_begin; 
				CBOnAryBegin m_cb_on_array_begin; 
				CBOnAryEnd m_cb_on_array_end; 
				CBOnNull m_cb_on_null; 
				CBOnIntegral m_cb_on_integral; 
				CBOnReal m_cb_on_real; 
				CBOnString m_cb_on_string; 
				CBOnBoolean m_cb_on_boolean;

				state_in_array_t( ):
						state_t{ },
						m_cb_on_object_begin{ },
						m_cb_on_array_begin{ }, 
						m_cb_on_array_end{ }, 
						m_cb_on_null{ }, 
						m_cb_on_integral{ }, 
						m_cb_on_real{ }, 
						m_cb_on_string{ }, 
						m_cb_on_boolean{ } { }

				state_in_array_t( CBOnObjBegin cb_on_object_begin, CBOnAryBegin cb_on_array_begin, CBOnAryEnd cb_on_array_end, CBOnNull cb_on_null, CBOnIntegral cb_on_integral, CBOnReal cb_on_real, CBOnString cb_on_string, CBOnBoolean cb_on_boolean ):
						state_t{ },
						m_cb_on_object_begin{ std::move( cb_on_object_begin ) },
						m_cb_on_array_begin{ std::move( cb_on_array_begin ) }, 
						m_cb_on_array_end{ std::move( cb_on_array_end ) }, 
						m_cb_on_null{ std::move( cb_on_null ) }, 
						m_cb_on_integral{ std::move( cb_on_integral ) }, 
						m_cb_on_real{ std::move( cb_on_real ) }, 
						m_cb_on_string{ std::move( cb_on_string ) }, 
						m_cb_on_boolean{ std::move( cb_on_boolean ) } { }


				state_in_array_t( state_in_array_t const & ) = default;
				state_in_array_t( state_in_array_t && ) = default;
				state_in_array_t & operator=( state_in_array_t const & ) = default;
				state_in_array_t & operator=( state_in_array_t && ) = default;

				~state_in_array_t( );

				void on_object_begin( ) override {
					m_cb_on_object_begin( );
				}

				void on_array_begin( ) override {
					m_cb_on_array_begin( );
				}

				void on_array_end( ) override {
					m_cb_on_array_end( );
				}

				void on_null( ) override {
					m_cb_on_null( );
				}

				void on_integral( boost::string_view value ) override {
					m_cb_on_integral( value );
				}

				void on_real( boost::string_view value ) override {
					m_cb_on_real( value );
				}

				void on_string( boost::string_view value ) override {
					m_cb_on_string( value );
				}

				void on_boolean( bool value ) override {
					m_cb_on_boolean( value );
				}
			};	// state_in_array_t

			template<typename CBOnObjBegin, typename CBOnAryBegin, typename CBOnAryEnd, typename CBOnNull, typename CBOnIntegral, typename CBOnReal, typename CBOnString, typename CBOnBoolean>
			state_in_array_t::~state_in_array_t( ) { }

			template<typename CBOnObjBegin, typename CBOnAryBegin, typename CBOnAryEnd, typename CBOnNull, typename CBOnIntegral, typename CBOnReal, typename CBOnString, typename CBOnBoolean>
			auto make_state_in_array_t( CBOnObjBegin cb_on_object_begin, CBOnAryBegin cb_on_array_begin, CBOnAryEnd cb_on_array_end, CBOnNull cb_on_null, CBOnIntegral cb_on_integral, CBOnReal cb_on_real, CBOnString cb_on_string, CBOnBoolean cb_on_boolean ) {
				return state_in_array_t<
							CBOnObjBegin, 
							CBOnAryBegin, 
							CBOnAryEnd, 
							CBOnNull, 
							CBOnIntegral, 
							CBOnReal, 
							CBOnString, 
							CBOnBoolean>{ std::move( cb_on_object_begin ), 
									std::move( cb_on_array_begin ), 
									std::move( cb_on_array_end ), 
									std::move( cb_on_null ), 
									std::move( cb_on_integral ),
									std::move( cb_on_real ),
									std::move( cb_on_string ),
									std::move( cb_on_boolean };
			}

			template<typename CBOnObjBegin, typename CBOnAryBegin>
			struct state_none_t: public state_t {
				CBOnObjBegin m_cb_on_object_begin;
				CBOnAryBegin m_cb_on_array_begin;

				state_none_t( ):
						state_t{ },
						m_cb_on_object_begin{ }
						m_cb_on_array_begin{ } { }

				state_none_t( CBOnObjBegin cb_on_object_begin, CBOnAryBegin cb_on_array_begin ):
						state_t{ },
						m_cb_on_object_begin{ std::move( cb_on_object_begin ) }
						m_cb_on_array_begin{ std::move( cb_on_array_begin ) } { }

				state_none_t( state_none_t const & ) = default;
				state_none_t( state_none_t && ) = default;
				state_none_t & operator=( state_none_t const & ) = default;
				state_none_t & operator=( state_none_t && ) = default;

				~state_none_t( );

				void on_object_begin( ) override;
				void on_array_begin( ) override;
			};	// state_none_t

			template<typename CBOnObjBegin, typename CBOnAryBegin>
			state_none_t::~state_none_t( ) { }

			template<typename CBOnObjBegin, typename CBOnAryBegin>
			auto make_state_none_t( CBOnObjBegin cb_on_object_begin, CB_on_array_begin ) {
				return state_none_t<CBOnObjBegin, CBOnAryBegin>{ std::move( cb_on_object_begin ), std::move( cb_on_array_begin ) };
			}
		}	// namspace state
	}	// namespace json
}    // namespace daw

