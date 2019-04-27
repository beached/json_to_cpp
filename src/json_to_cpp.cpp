// The MIT License (MIT)
//
// Copyright (c) 2016-2019 Darrell Wright
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

#include <algorithm>
#include <limits>
#include <string>
#include <typeindex>

#include <daw/daw_bounded_hash_set.h>
#include <daw/daw_string_view.h>
#include <daw/json/daw_json_parser.h>

#include "json_to_cpp.h"
#include "ti_types.h"
#include "ti_value.h"

namespace daw::json_to_cpp {
	namespace {
		struct state_t {
			bool has_arrays = false;
			bool has_integrals = false;
			bool has_optionals = false;
			bool has_strings = false;
			bool has_kv = false;
			std::vector<std::string> path = {};
		};

		bool is_valid_id_char( char c ) noexcept {
			return ( std::isalnum( c ) != 0 ) or c == '_';
		}

		std::string find_replace( std::string subject, std::string const &search,
		                          std::string const &replace ) {
			size_t pos = 0;
			while( ( pos = subject.find( search, pos ) ) != std::string::npos ) {
				subject.replace( pos, search.length( ), replace );
				pos += replace.length( );
			}
			return subject;
		}

		/// Add a "json_" prefix to C++ keywords and escape non representable
		/// characters
		///
		/// \param name Name of identifier
		/// \return A valid C++ identifier
		std::string make_compliant_names( std::string name ) {
			// These identifiers cannot be used in c++, we will prefix them to keep
			// them from colliding with keywords
			// clang-format off
			static constexpr auto keywords =
					daw::make_bounded_hash_set<daw::string_view>( {
							"alignas", "alignof", "and", "and_eq", "asm",	"atomic_cancel", "atomic_commit",
							"atomic_noexcept", "auto", "bitand", "bitor", "bool",	"break", "case", "catch",
							"char", "char16_t", "char32_t", "class", "compl", "concept", "const", "constexpr",
							"const_cast", "continue", "decltype", "default", "delete", "do", "double", "dynamic_cast",
							"else", "enum", "explicit", "export", "extern", "false", "float", "for", "friend",
							"goto", "if", "import", "", "int", "long", "module", "mutable", "namespace",
							"new", "noexcept", "not", "not_eq", "nullptr", "operator", "or", "or_eq", "private",
							"protected", "public", "register", "reinterpret_cast", "requires", "return", "short",
							"signed", "sizeof", "static", "static_assert", "static_cast", "struct", "switch", "synchronized",
							"template", "this", "thread_local", "throw", "true", "try", "typedef", "typeid", "typename",
							"union", "unsigned", "using", "virtual", "void", "volatile", "wchar_t", "while", "xor", "xor_eq"} );
			// clang-format on
			// Remove escaped things
			name = find_replace( name, "\\U", "0x" );
			name = find_replace( name, "\\u", "0x" );
			// JSON member names are strings.  That is it, so empty looks
			// like it is valid, as is all digits, or C++ keyworks.
			if( name.empty( ) or
			    !( std::isalpha( name.front( ) ) or name.front( ) == '_' ) or
			    keywords.count( {name.data( ), name.size( )} ) > 0 ) {

				std::string const prefix = "_json";
				name.insert( name.begin( ), prefix.begin( ), prefix.end( ) );
			}
			// Look for characters that are not in the basic standard 5.10
			// non-digit or digit and escape them
			auto new_name = std::string( );
			daw::algorithm::transform_it(
			  name.begin( ), name.end( ), std::back_inserter( new_name ),
			  []( char c, auto it ) {
				  if( !is_valid_id_char( c ) ) {
					  std::string const new_value =
					    "0x" + std::to_string( static_cast<int>( c ) );
					  it = std::copy( new_value.begin( ), new_value.end( ), it );
				  } else {
					  *it++ = c;
				  }
				  return it;
			  } );
			return new_name;
		}

		std::vector<types::ti_object>::iterator
		find_by_name( std::vector<types::ti_object> &obj_info,
		              daw::string_view name ) {
			return std::find_if( obj_info.begin( ), obj_info.end( ),
			                     [n = name.to_string( )]( auto const &item ) {
				                     return n == item.name( );
			                     } );
		}

		template<typename Variant>
		constexpr bool is_double( Variant &&v ) noexcept {
			return std::holds_alternative<types::ti_real>( v );
		}

		struct is_optional_visitor {
			bool &operator( )( types::ti_array &item ) const {
				return item.is_optional;
			}
			bool operator( )( types::ti_array const &item ) const {
				return item.is_optional;
			}

			bool &operator( )( types::ti_boolean &item ) const {
				return item.is_optional;
			}
			bool operator( )( types::ti_boolean const &item ) const {
				return item.is_optional;
			}

			bool &operator( )( types::ti_integral &item ) const {
				return item.is_optional;
			}
			bool operator( )( types::ti_integral const &item ) const {
				return item.is_optional;
			}

			bool &operator( )( types::ti_null &item ) const {
				return item.is_optional;
			}
			bool operator( )( types::ti_null const &item ) const {
				return item.is_optional;
			}

			bool &operator( )( types::ti_object &item ) const {
				return item.is_optional;
			}
			bool operator( )( types::ti_object const &item ) const {
				return item.is_optional;
			}

			bool &operator( )( types::ti_kv &item ) const {
				return item.is_optional;
			}

			bool operator( )( types::ti_kv const &item ) const {
				return item.is_optional;
			}

			bool &operator( )( types::ti_real &item ) const {
				return item.is_optional;
			}
			bool operator( )( types::ti_real const &item ) const {
				return item.is_optional;
			}

			bool &operator( )( types::ti_string &item ) const {
				return item.is_optional;
			}
			bool operator( )( types::ti_string const &item ) const {
				return item.is_optional;
			}
		};

		template<typename Variant>
		constexpr decltype( auto ) is_optional( Variant &&v ) {
			return daw::visit_nt( std::forward<Variant>( v ), is_optional_visitor{} );
		}

		template<typename Variant>
		constexpr decltype( auto ) is_null( Variant &&v ) {
			return daw::visit_nt( std::forward<Variant>( v ),
			                      []( auto &&item ) { return item.is_null; } );
		}

		void add_or_merge( std::vector<types::ti_object> &obj_info,
		                   types::ti_object &obj ) {
			auto pos =
			  find_by_name( obj_info, {obj.name( ).data( ), obj.name( ).size( )} );
			if( obj_info.end( ) == pos ) {
				// First time
				obj_info.push_back( obj );
				return;
			}

			auto diff = std::vector<std::pair<std::string, types::ti_value>>( );
			for( auto &orig_child : *pos->children ) {
				auto child_pos = std::find_if(
				  obj.children->begin( ), obj.children->end( ),
				  [&]( auto const &v ) { return v.first == orig_child.first; } );
				if( child_pos == obj.children->end( ) ) {
					is_optional( orig_child.second ) = true;
					continue;
				}
				if( is_null( child_pos->second ) ) {
					is_optional( orig_child.second ) = true;
				} else if( is_null( orig_child.second ) ) {
					orig_child.second = child_pos->second;
					is_optional( orig_child.second ) = true;
				} else if( is_double( child_pos->second ) ) {
					// Account for when the LHS is an int but the value should actually
					// be a double
					auto const is_opt = is_optional( orig_child.second );
					orig_child.second = orig_child.second;
					is_optional( orig_child.second ) =
					  is_optional( orig_child.second ) or is_opt;
				}
			}
		}

		types::ti_types_t merge_array_values( types::ti_value a,
		                                      types::ti_value b ) {
			if( a.is_null( ) ) {
				b.is_optional( ) = true;
				return b.value;
			}
			a.is_optional( ) |= b.is_optional( );
			return a.value;
		}

		types::ti_types_t
		parse_json_object( daw::json::json_value_t const &current_item,
		                   daw::string_view cur_name,
		                   std::vector<types::ti_object> &obj_info,
		                   state_t &obj_state, config_t const &config ) {

			using ::daw::json::json_value_t;
			using namespace ::daw::json_to_cpp::types;
			if( current_item.is_integer( ) ) {
				obj_state.has_integrals = true;
				return ti_integral( );
			}
			if( current_item.is_real( ) ) {
				return ti_real( );
			}
			if( current_item.is_boolean( ) ) {
				return ti_boolean( );
			}
			if( current_item.is_string( ) ) {
				obj_state.has_strings = true;
				return ti_string( config.use_string_view );
			}
			if( current_item.is_null( ) ) {
				obj_state.has_optionals = true;
				return ti_null( );
			}
			if( current_item.is_object( ) ) {
				obj_state.path.push_back( cur_name );
				// TODO key_value
				auto result = ti_object( cur_name.to_string( ) + "_t" );
				for( auto const &child : current_item.get_object( ) ) {
					std::string const child_name =
					  make_compliant_names( child.first.to_string( ) );
					( *result.children )[child_name] = parse_json_object(
					  child.second, child_name, obj_info, obj_state, config );
				}
				add_or_merge( obj_info, result );
				return result;
			}
			if( current_item.is_array( ) ) {
				obj_state.has_arrays = true;
				auto result = ti_array( );
				auto arry = current_item.get_array( );
				auto const child_name = cur_name.to_string( ) + "_element";
				if( arry.empty( ) ) {
					( *result.children )[child_name] = ti_null( );
				} else {
					auto const last_item = arry.back( );
					arry.pop_back( );
					auto child = parse_json_object( last_item, child_name, obj_info,
					                                obj_state, config );
					for( auto const &element : current_item.get_array( ) ) {
						child = merge_array_values(
						  ti_value( child ),
						  ti_value( parse_json_object( element, child_name, obj_info,
						                               obj_state, config ) ) );
					}
					( *result.children )[child_name] = child;
				}
				return result;
			}
			std::cerr << "Unexpected exit point to parse_json_object2";
			std::terminate( );
		}

		std::vector<types::ti_object>
		parse_json_object( daw::json::json_value_t const &current_item,
		                   state_t &obj_state, config_t const &config ) {
			using namespace daw::json;
			auto result = std::vector<types::ti_object>( );

			if( current_item.is_object( ) ) {
				parse_json_object( current_item, "root_object", result, obj_state,
				                   config );
			} else {
				auto root_obj_member =
				  ::daw::json::make_object_value_item( "root_obj", current_item );
				auto root_object = json_object_value( );
				root_object.members_v.push_back( std::move( root_obj_member ) );
				auto root_value = json_value_t( std::move( root_object ) );
				parse_json_object( root_value, "root_object", result, obj_state,
				                   config );
			}
			return result;
		}

		void generate_json_link_maps( std::integral_constant<int, 3>,
		                              bool definition, config_t &config,
		                              types::ti_object const &cur_obj ) {
			if( !config.enable_jsonlink ) {
				return;
			}
			if( !definition ) {
				using daw::json::json_value_t;
				config.cpp_file( ) << "inline auto describe_json_class( "
				                   << cur_obj.object_name
				                   << " ) {\n\tusing namespace daw::json;\n";
				for( auto const &child : *cur_obj.children ) {
					if( config.hide_null_only and is_null( child.second ) ) {
						continue;
					}
					config.cpp_file( )
					  << "\tstatic constexpr char const " << child.first << "[] = \"";
					if( daw::string_view( child.first.data( ), child.first.size( ) )
					      .starts_with( "_json" ) ) {
						config.cpp_file( )
						  << daw::string_view( child.first.data( ), child.first.size( ) )
						       .substr( 5 )
						  << "\";\n";
					} else {
						config.cpp_file( ) << child.first << "\";\n";
					}
				}

				config.cpp_file( ) << "\treturn daw::json::class_description_t<\n";

				bool is_first = true;

				for( auto const &child : *cur_obj.children ) {
					if( config.hide_null_only and is_null( child.second ) ) {
						continue;
					}
					config.cpp_file( ) << "\t\t";
					if( !is_first ) {
						config.cpp_file( ) << ",";
					} else {
						is_first = false;
					}
					if( is_optional( child.second ) ) {
						config.cpp_file( ) << "json_nullable<";
					}
					config.cpp_file( )
					  << types::ti_value( child.second ).json_name( child.first );
					if( is_optional( child.second ) ) {
						config.cpp_file( ) << ">\n";
					} else {
						config.cpp_file( ) << '\n';
					}
				}
				config.cpp_file( ) << ">{};\n}\n\n";

				config.cpp_file( ) << " inline auto to_json_data( "
				                   << cur_obj.object_name << " const & value ) {\n";
				config.cpp_file( ) << "\treturn std::forward_as_tuple( ";
				is_first = true;
				for( auto const &child : *cur_obj.children ) {
					if( config.hide_null_only and is_null( child.second ) ) {
						continue;
					}
					if( !is_first ) {
						config.cpp_file( ) << ", ";
					} else {
						is_first = !is_first;
					}
					config.cpp_file( ) << "value." << child.first;
				}
				config.cpp_file( ) << " );\n}\n\n";
			}
		}

		void generate_json_link_maps( bool definition, config_t &config,
		                              types::ti_object const &cur_obj ) {

			return generate_json_link_maps( std::integral_constant<int, 3>( ),
			                                definition, config, cur_obj );
		}

		void generate_includes( bool definition, config_t &config,
		                        state_t const &obj_state ) {
			{
				std::string const header_message =
				  "// Code auto generated from json file '" +
				  config.json_path.string( ) + "'\n\n";
				if( !definition ) {
					config.header_file( ) << header_message;
				}
			}
			if( !definition ) {
				config.header_file( ) << "#pragma once\n\n";
				config.header_file( ) << "#include <tuple>\n";
				if( obj_state.has_optionals ) {
					config.header_file( ) << "#include <optional>\n";
				}
				if( obj_state.has_integrals ) {
					config.header_file( ) << "#include <cstdint>\n";
				}
				if( obj_state.has_strings ) {
					if( config.use_string_view ) {
						config.header_file( ) << "#include <string_view>\n";
					} else {
						config.header_file( ) << "#include <string>\n";
					}
				}
				if( obj_state.has_arrays ) {
					config.header_file( ) << "#include <vector>\n";
				}
				if( config.enable_jsonlink ) {
					config.header_file( ) << "#include <daw/json/daw_json_link.h>\n";
				}
				config.header_file( ) << '\n';
			}
		}

		void generate_declarations( std::vector<types::ti_object> const &obj_info,
		                            config_t &config ) {
			for( auto const &cur_obj : obj_info ) {
				auto const obj_type = cur_obj.name( );
				config.header_file( ) << "struct " << obj_type << " {\n";
				for( auto const &child : *cur_obj.children ) {
					if( config.hide_null_only and is_null( child.second ) ) {
						continue;
					}
					auto const &member_name = child.first;
					auto const &member_type = types::ti_value( child.second ).name( );
					config.header_file( ) << "\t";
					if( is_optional( child.second ) ) {
						config.header_file( ) << "std::optional<" << member_type << ">";
					} else {
						config.header_file( ) << member_type;
					}
					config.header_file( ) << " " << member_name << ";\n";
				}
				config.header_file( ) << "};"
				                      << "\t// " << obj_type << "\n\n";
				if( config.enable_jsonlink ) {
					generate_json_link_maps( false, config, cur_obj );
				}
			}
		}

		void generate_definitions( std::vector<types::ti_object> const &obj_info,
		                           config_t &config ) {
			if( !config.enable_jsonlink ) {
				return;
			}
			for( auto const &cur_obj : obj_info ) {
				generate_json_link_maps( true, config, cur_obj );
			}
		}

		void generate_code( std::vector<types::ti_object> const &obj_info,
		                    config_t &config, state_t const &obj_state ) {
			generate_includes( true, config, obj_state );
			generate_includes( false, config, obj_state );
			generate_declarations( obj_info, config );
			generate_definitions( obj_info, config );
		}
	} // namespace

	std::ostream &config_t::header_file( ) {
		return *header_stream;
	}

	std::ostream &config_t::cpp_file( ) {
		return *cpp_stream;
	}

	void generate_cpp( daw::string_view json_string, config_t &config ) {
		auto obj_state = state_t( );
		auto json_obj = ::daw::json::parse_json( json_string );
		auto obj_info = parse_json_object( json_obj, obj_state, config );
		generate_code( obj_info, config, obj_state );
	}
} // namespace daw::json_to_cpp
