// The MIT License (MIT)
//
// Copyright (c) 2014-2017 Darrell Wright
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
#include <cctype>
#include <exception>
#include <functional>
#include <string>

#include <daw/daw_parser_helper.h>
#include <daw/daw_traits.h>

namespace daw {
	namespace json {
		namespace impl {
			struct json_parser_exception final : public std::runtime_error {
				json_parser_exception( std::string message ) : std::runtime_error{std::move( message )} {}

				~json_parser_exception( );
				json_parser_exception( json_parser_exception const & ) = default;
				json_parser_exception( json_parser_exception && ) = default;
				json_parser_exception &operator=( json_parser_exception const & ) = default;
				json_parser_exception &operator=( json_parser_exception && ) = default;
			}; // struct json_parser_exception

			template<typename InputIterator, typename State>
			auto parse_integral( InputIterator &first, InputIterator const &last, State &state ) {
				if( first == last || !isdigit( *first ) ) {
					throw json_parser_exception( "Expecting digits, found none '" + state.buffer + "'" );
				}
				++first;
				while( first != last && isdigit( *first ) ) {
					++first;
				}
				return first;
			}

			template<typename InputIterator>
			void throw_at_end_of_stream( InputIterator const &pos, InputIterator const &last,
			                             boost::string_view message ) {
				if( pos == last ) {
					throw json_parser_exception{message.to_string( )};
				}
			}

			template<typename CHR>
			constexpr bool is_insignificant_ws( CHR const c ) noexcept {
				return c == 0x20 || c == 0x09 || c == 0x0A || c == 0x0D;
			}

			template<typename InputIterator>
			auto skip_whitespace( InputIterator &first, InputIterator const &last ) noexcept {
				while( first != last && is_insignificant_ws( *first ) ) {
					++first;
				}
				return first;
			}

			template<typename InputIterator, typename State>
			auto parse_number( InputIterator &first, InputIterator const &last, State &state ) {
				using namespace daw::parser;
				// Assume that *first is in the set { '-', '0'->'9' }

				if( '-' == *first ) {
					++first;
				}
				first = parse_integral( first, last, state );

				if( first == last || !is_a( *first, '.', 'E', 'e' ) ) {
					state.on_integral( );
					return first;
				}
				if( '.' == *first ) {
					++first;
					first = parse_integral( first, last, state );
				}
				if( first == last || !is_a( *first, 'e', 'E' ) ) {
					state.on_real( );
					return first;
				}
				++first;
				if( is_a( *first, '-', '+' ) ) {
					++first;
				}
				first = parse_integral( first, last, state );
				state.on_real( );
				return first;
			}

			template<typename InputIterator, typename State>
			auto parse_string( InputIterator &first, InputIterator const &last, State &state ) {
				// Assumes that *first == '"'
				++first;
				state.clear_buffer( );
				bool in_slash = false;
				while( first != last ) {
					switch( *first ) {
					case '"':
						if( !in_slash ) {
							state.on_string( state.buffer );
							return std::next( first );
						}
						break;
					case '\\':
						in_slash = true;
						break;
					default:
						in_slash = false;
						break;
					}
					state.push( *first );
					++first;
				}
				throw json_parser_exception( "Unclosed string when end of input reached" );
			}

			template<typename InputIterator, typename State>
			auto parse_true( InputIterator &first, InputIterator const &last, State &state ) {
				++first;
				if( first == last || 'r' != *( first++ ) )
					throw json_parser_exception( "Expected boolean true" );
				if( first == last || 'u' != *( first++ ) )
					throw json_parser_exception( "Expected boolean true" );
				if( first == last || 'e' != *( first++ ) )
					throw json_parser_exception( "Expected boolean true" );
				state.on_boolean( true );
				return first;
			}

			template<typename InputIterator, typename State>
			auto parse_false( InputIterator &first, InputIterator const &last, State &state ) {
				++first;
				if( first == last || 'a' != *( first++ ) )
					throw json_parser_exception( "Expected boolean true" );
				if( first == last || 'l' != *( first++ ) )
					throw json_parser_exception( "Expected boolean true" );
				if( first == last || 's' != *( first++ ) )
					throw json_parser_exception( "Expected boolean true" );
				if( first == last || 'e' != *( first++ ) )
					throw json_parser_exception( "Expected boolean true" );
				state.on_boolean( false );
				return first;
			}

			template<typename InputIterator, typename State>
			auto parse_null( InputIterator &first, InputIterator const &last, State &state ) {
				++first;
				if( first == last || 'u' != *( first++ ) )
					throw json_parser_exception( "Expected boolean true" );
				if( first == last || 'l' != *( first++ ) )
					throw json_parser_exception( "Expected boolean true" );
				if( first == last || 'l' != *( first++ ) )
					throw json_parser_exception( "Expected boolean true" );
				state.on_null( );
				return first;
			}

			template<typename InputIterator, typename State>
			InputIterator parse_object( InputIterator &, InputIterator const &, State & );
			template<typename InputIterator, typename State>
			InputIterator parse_array( InputIterator &, InputIterator const &, State & );

			template<typename InputIterator, typename State>
			auto parse_value( InputIterator &first, InputIterator const &last, State &state ) {
				// Assume that *first == '{'
				first = skip_whitespace( first, last );
				throw_at_end_of_stream( first, last, "Unexpected end of stream" );
				switch( *first ) {
				case 't':
					first = parse_true( first, last, state );
					break;
				case 'f':
					first = parse_false( first, last, state );
					break;
				case 'n':
					first = parse_null( first, last, state );
					break;
				case '"':
					first = parse_string( first, last, state );
					break;
				case '{':
					first = parse_object( first, last, state );
					break;
				case '[':
					first = parse_array( first, last, state );
					break;
				case '-':
				case '0':
				case '1':
				case '2':
				case '3':
				case '4':
				case '5':
				case '6':
				case '7':
				case '8':
				case '9':
					first = parse_number( first, last, state );
					break;
				default:
					// Unrecognized character
					throw json_parser_exception( "Unexpected character" );
				}
				return first;
			}

			template<typename InputIterator, typename State>
			InputIterator parse_array( InputIterator &first, InputIterator const &last, State &state ) {
				// Assume the *first == '['
				state.on_array_begin( );
				++first;
				first = skip_whitespace( first, last );
				while( first != last && *first != ']' ) {
					first = parse_value( first, last, state );
					first = skip_whitespace( first, last );
					throw_at_end_of_stream( first, last, "Expected a closing ']' for array but found end" );
					if( *first == ']' ) {
						state.on_array_end( );
						return std::next( first );
					} else if( *first != ',' ) {
						throw json_parser_exception( "Expected a comma" );
					}
					// We're a comma, move to next element
					++first;
					first = skip_whitespace( first, last );
				}
				throw json_parser_exception( "Expected a closing ']' for array but found end" );
			}

			template<typename InputIterator, typename State>
			InputIterator parse_object( InputIterator &first, InputIterator const &last, State &state ) {
				// Assume the *first == '{'
				state.on_object_begin( );
				++first;
				first = skip_whitespace( first, last );
				while( first != last && *first != '}' ) {
					first = parse_string( first, last, state );
					first = skip_whitespace( first, last );
					throw_at_end_of_stream( first, last, "Expected member separator ':' but found end" );
					if( *first != ':' ) {
						throw json_parser_exception( "Unexpected character.  Expecting ':'" );
					}
					++first;
					first = parse_value( first, last, state );
					first = skip_whitespace( first, last );
					throw_at_end_of_stream( first, last,
					                        "Expected a closing '}' or a ',' for next member but found end" );
					if( *first == '}' ) {
						state.on_object_end( );
						return std::next( first );
					} else if( *first != ',' ) {
						throw json_parser_exception( "Expected a comma" );
					}
					// We're a comma, move to next element
					++first;
					first = skip_whitespace( first, last );
				}
				throw json_parser_exception( "Expected a closing '}' for object" );
			}

		} // namespace impl

		template<typename InputIterator, typename State>
		void json_parser( InputIterator first, InputIterator last, State &state ) {
			// impl::parse_value( first, last, state );
			using namespace impl;
			first = skip_whitespace( first, last );
			while( first != last ) {
				switch( *first ) {
				case '{':
					first = parse_object( first, last, state );
					break;
				case '[':
					first = parse_array( first, last, state );
					break;
				default:
					throw json_parser_exception( "Expected an array '[' or object '{' start" );
				}
				first = skip_whitespace( first, last );
			}
		}

	} // namespace json
} // namespace daw
