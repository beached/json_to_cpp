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
#include <boost/utility/string_view.hpp>
#include <limits>
#include <map>
#include <string>
#include <tuple>
#include <unordered_map>
#include <unordered_set>

#include <daw/json/daw_json_link.h>

#include "cpp_generator.h"
#include "json_to_cpp.h"
#include "ti_value.h"

namespace daw {
	namespace json_to_cpp {
		std::ostream &config_t::header_file( ) {
			return *header_stream;
		}

		std::ostream &config_t::cpp_file( ) {
			return *cpp_stream;
		}

		namespace {
			/// Add a "json_" prefix to C++ keywords
			std::string replace_keywords( std::string name ) {
				static std::unordered_set<std::string> const keywords = {"alignas",
				                                                         "alignof",
				                                                         "and",
				                                                         "and_eq",
				                                                         "asm",
				                                                         "atomic_cancel",
				                                                         "atomic_commit",
				                                                         "atomic_noexcept",
				                                                         "auto",
				                                                         "bitand",
				                                                         "bitor",
				                                                         "bool",
				                                                         "break",
				                                                         "case",
				                                                         "catch",
				                                                         "char",
				                                                         "char16_t",
				                                                         "char32_t",
				                                                         "class",
				                                                         "compl",
				                                                         "concept",
				                                                         "const",
				                                                         "constexpr",
				                                                         "const_cast",
				                                                         "continue",
				                                                         "decltype",
				                                                         "default",
				                                                         "delete",
				                                                         "do",
				                                                         "double",
				                                                         "dynamic_cast",
				                                                         "else",
				                                                         "enum",
				                                                         "explicit",
				                                                         "export",
				                                                         "extern",
				                                                         "false",
				                                                         "float",
				                                                         "for",
				                                                         "friend",
				                                                         "goto",
				                                                         "if",
				                                                         "import",
				                                                         "inline",
				                                                         "int",
				                                                         "long",
				                                                         "module",
				                                                         "mutable",
				                                                         "namespace",
				                                                         "new",
				                                                         "noexcept",
				                                                         "not",
				                                                         "not_eq",
				                                                         "nullptr",
				                                                         "operator",
				                                                         "or",
				                                                         "or_eq",
				                                                         "private",
				                                                         "protected",
				                                                         "public",
				                                                         "register",
				                                                         "reinterpret_cast",
				                                                         "requires",
				                                                         "return",
				                                                         "short",
				                                                         "signed",
				                                                         "sizeof",
				                                                         "static",
				                                                         "static_assert",
				                                                         "static_cast",
				                                                         "struct",
				                                                         "switch",
				                                                         "synchronized",
				                                                         "template",
				                                                         "this",
				                                                         "thread_local",
				                                                         "throw",
				                                                         "true",
				                                                         "try",
				                                                         "typedef",
				                                                         "typeid",
				                                                         "typename",
				                                                         "union",
				                                                         "unsigned",
				                                                         "using",
				                                                         "virtual",
				                                                         "void",
				                                                         "volatile",
				                                                         "wchar_t",
				                                                         "while",
				                                                         "xor",
				                                                         "xor_eq"};

				if( keywords.count( name ) > 0 ) {
					name = "json_" + name;
				}
				return name;
			}

			std::vector<types::ti_object>::iterator find_by_name( std::vector<types::ti_object> &obj_info,
			                                                      boost::string_view name ) {
				return std::find_if( obj_info.begin( ), obj_info.end( ), [n = name.to_string( )]( auto const &item ) {
					return n == item.name( );
				} );
			}

			void add_or_merge( std::vector<types::ti_object> &obj_info, types::ti_object const &obj ) {
				auto pos = find_by_name( obj_info, obj.name( ) );
				if( obj_info.end( ) == pos ) {
					// First time
					obj_info.push_back( obj );
					return;
				}

				types::ti_object &orig = *pos;
				std::vector<std::pair<std::string, types::ti_value>> diff;

				static auto const comp = []( auto const &c1, auto const &c2 ) { return c1.first < c2.first; };
				std::set_difference( orig.children.begin( ), orig.children.end( ), obj.children.begin( ),
				                     obj.children.end( ), std::back_inserter( diff ), comp );

				for( auto &child : diff ) {
					child.second.is_optional( ) = true;
					orig.children[child.first] = child.second;
				}
			}

			types::ti_value merge_array_values( types::ti_value const &a, types::ti_value const &b ) {
				using daw::json::impl::value_t;
				types::ti_value result = b;
				if( a.type( ) == value_t::value_types::null ) {
					result.is_optional( ) = true;
				} else if( b.type( ) == value_t::value_types::null ) {
					result = a;
					result.is_optional( ) = true;
				}
				return result;
			}

			types::ti_value parse_json_object( daw::json::impl::value_t const &current_item,
			                                   boost::string_view cur_name, std::vector<types::ti_object> &obj_info,
			                                   object_state_t &obj_state ) {
				using daw::json::impl::value_t;
				switch( current_item.type( ) ) {
				case value_t::value_types::integral:
					obj_state.has_integrals = true;
					return types::create_ti_value<types::ti_integral>( );
				case value_t::value_types::real:
					return types::create_ti_value<types::ti_real>( );
				case value_t::value_types::boolean:
					return types::create_ti_value<types::ti_boolean>( );
				case value_t::value_types::string:
					obj_state.has_strings = true;
					return types::create_ti_value<types::ti_string>( );
				case value_t::value_types::null:
					obj_state.has_optionals = true;
					return types::create_ti_value<types::ti_null>( );
				case value_t::value_types::object: {
					auto result = types::ti_object{cur_name.to_string( ) + "_t"};
					for( auto const &child : current_item.get_object( ) ) {
						std::string const child_name = replace_keywords( child.first.to_string( ) );
						result.children[child_name] =
						    parse_json_object( child.second, child_name, obj_info, obj_state );
					}
					add_or_merge( obj_info, result );
					return result;
				}
				case value_t::value_types::array: {
					obj_state.has_arrays = true;
					auto result = types::create_ti_value<types::ti_array>( );
					auto arry = current_item.get_array( );
					auto const child_name = cur_name.to_string( ) + "_element";
					if( arry.empty( ) ) {
						result.children( )[child_name] = types::create_ti_value<types::ti_null>( );
					} else {
						auto const last_item = arry.back( );
						arry.pop_back( );
						auto child = parse_json_object( last_item, child_name, obj_info, obj_state );
						for( auto const &element : current_item.get_array( ) ) {
							child = merge_array_values( child,
							                            parse_json_object( element, child_name, obj_info, obj_state ) );
						}
						result.children( )[child_name] = child;
					}
					return result;
				}
				}
				throw std::runtime_error( "Unexpected exit point to parse_json_object2" );
			}

			std::vector<types::ti_object> parse_json_object( daw::json::impl::value_t const &current_item,
			                                                 object_state_t &obj_state ) {
				using namespace daw::json::impl;
				std::vector<types::ti_object> result;

				if( !current_item.is_object( ) ) {
					auto root_obj_member = daw::json::impl::make_object_value_item( "root_obj", current_item );
					object_value root_object;
					root_object.members_v.push_back( std::move( root_obj_member ) );
					value_t root_value{std::move( root_object )};
					parse_json_object( root_value, "root_object", result, obj_state );
				} else {
					parse_json_object( current_item, "root_object", result, obj_state );
				}
				return result;
			}

		} // namespace

		void json_to_cpp( boost::string_view json_string, config_t &config ) {
			object_state_t obj_state;
			auto json_obj = daw::json::parse_json( json_string );
			auto obj_info = parse_json_object( json_obj, obj_state );
			generate::generate_code( obj_info, config, obj_state );
		}
	} // namespace json_to_cpp
} // namespace daw

