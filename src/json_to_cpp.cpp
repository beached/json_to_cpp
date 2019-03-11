// The MIT License (MIT)
//
// Copyright (c) 2016 Darrell Wright
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
#include <map>
#include <string>
#include <tuple>
#include <typeindex>
#include <unordered_map>
#include <unordered_set>

#include <daw/daw_bounded_hash_set.h>
#include <daw/daw_string_view.h>

#include "json_to_cpp.h"
#include <daw/json/daw_json_parser.h>
#include <daw/json/daw_json_parser_v2.h>
#include <daw/json/daw_json_value_t.h>

namespace daw {
	namespace json_to_cpp {
		std::ostream &config_t::header_file( ) {
			return *header_stream;
		}

		std::ostream &config_t::cpp_file( ) {
			return *cpp_stream;
		}

		struct state_t {
			bool has_arrays = false;
			bool has_integrals = false;
			bool has_optionals = false;
			bool has_strings = false;
		};

		namespace {
			/// Add a "json_" prefix to C++ keywords
			std::string replace_keywords( std::string name ) {
				static constexpr auto keywords =
				  daw::make_bounded_hash_set<daw::string_view>( {"alignas",
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
				                                                 "xor_eq"} );

				if( keywords.count( {name.data( ), name.size( )} ) > 0 ) {
					std::string const prefix = "_json";
					name.insert( name.begin( ), prefix.begin( ), prefix.end( ) );
				}
				return name;
			}

			namespace types {

				struct type_info_t;

				struct ti_value {
					type_info_t *value = nullptr;

					std::string name( ) const noexcept;
					std::string json_name( std::string member_name ) const noexcept;
					std::map<std::string, ti_value> const &children( ) const;
					std::map<std::string, ti_value> &children( );
					bool &is_optional( ) noexcept;
					bool const &is_optional( ) const noexcept;

					size_t type( ) const;

					inline bool is_null( ) const {
						return type( ) ==
						       json::json_value_t::index_of<json::json_value_t::null_t>( );
					}

					constexpr ti_value( ) noexcept = default;

					~ti_value( );

					ti_value( ti_value const &other );

					inline ti_value( ti_value &&other ) noexcept
					  : value{std::exchange( other.value, nullptr )} {}

					inline ti_value &operator=( ti_value const &rhs ) {
						if( this != &rhs ) {
							ti_value tmp{rhs};
							using std::swap;
							swap( *this, tmp );
						}
						return *this;
					}

					ti_value &operator=( ti_value &&rhs ) {
						if( this != &rhs ) {
							value = std::exchange( rhs.value, nullptr );
						}
						return *this;
					}

					template<typename Derived>
					ti_value( Derived other )
					  : value{new Derived( std::move( other ) )} {}

					template<typename Derived>
					ti_value &operator=( Derived rhs ) {
						value = new Derived( std::move( rhs ) );
						return *this;
					}
				};

				template<typename Derived, typename... Args>
				ti_value create_ti_value( Args &&... args ) {
					ti_value result;
					auto tmp = new Derived( std::forward<Args>( args )... );
					result.value = std::exchange( tmp, nullptr );
					return result;
				}

				struct type_info_t {
					std::map<std::string, ti_value> children{};
					bool is_optional = false;

					type_info_t( ) = default;
					type_info_t( type_info_t const & ) = default;
					type_info_t( type_info_t && ) noexcept = default;
					type_info_t &operator=( type_info_t const & ) = default;
					type_info_t &operator=( type_info_t && ) noexcept = default;
					virtual ~type_info_t( );

					virtual size_t type( ) const = 0;

					bool is_null( ) const {
						return this->type( ) ==
						       json::json_value_t::index_of<json::json_value_t::null_t>( );
					}
					virtual std::string name( ) const = 0;
					virtual std::string json_name( std::string member_name ) const = 0;
					virtual type_info_t *clone( ) const = 0;
				}; // type_info_t

				type_info_t::~type_info_t( ) {}

				std::string ti_value::name( ) const noexcept {
					return value->name( );
				}

				std::string ti_value::json_name( std::string member_name ) const
				  noexcept {
					return value->json_name( std::move( member_name ) );
				}

				size_t ti_value::type( ) const {
					return value->type( );
				}

				std::map<std::string, ti_value> const &ti_value::children( ) const {
					return value->children;
				}

				std::map<std::string, ti_value> &ti_value::children( ) {
					return value->children;
				}

				bool &ti_value::is_optional( ) noexcept {
					return value->is_optional;
				}

				bool const &ti_value::is_optional( ) const noexcept {
					return value->is_optional;
				}

				ti_value::~ti_value( ) {
					delete std::exchange( value, nullptr );
				}

				ti_value::ti_value( ti_value const &other )
				  : value{other.value->clone( )} {}

				struct ti_null : public type_info_t {
					size_t type( ) const override {
						return daw::json::json_value_t::index_of<
						  daw::json::json_value_t::null_t>( );
					}

					std::string name( ) const override {
						return "void*";
					}

					std::string json_name( std::string member_name ) const override {
						return "json_custom<" + member_name + ">";
					}

					ti_null( )
					  : type_info_t{} {
						is_optional = true;
					}

					type_info_t *clone( ) const override {
						return new ti_null( *this );
					}
				};

				struct ti_integral : public type_info_t {
					size_t type( ) const override {
						return daw::json::json_value_t::index_of<
						  daw::json::json_value_t::integer_t>( );
					}

					std::string name( ) const override {
						return "int64_t";
					}

					std::string json_name( std::string member_name ) const override {
						return "json_number<" + member_name + ", intmax_t>";
					}

					type_info_t *clone( ) const override {
						return new ti_integral( *this );
					}
				};

				struct ti_real : public type_info_t {
					size_t type( ) const override {
						return daw::json::json_value_t::index_of<
						  daw::json::json_value_t::real_t>( );
					}

					std::string name( ) const override {
						return "double";
					}

					std::string json_name( std::string member_name ) const override {
						return "json_number<" + member_name + ">";
					}

					type_info_t *clone( ) const override {
						return new ti_real( *this );
					}
				};

				struct ti_boolean : public type_info_t {
					size_t type( ) const override {
						return daw::json::json_value_t::index_of<
						  daw::json::json_value_t::boolean_t>( );
					}

					std::string name( ) const override {
						return "bool";
					}

					std::string json_name( std::string member_name ) const override {
						return "json_bool<" + member_name + ">";
					}

					type_info_t *clone( ) const override {
						return new ti_boolean( *this );
					}
				};

				struct ti_string : public type_info_t {
					size_t type( ) const override {
						return daw::json::json_value_t::index_of<
						  daw::json::json_value_t::string_t>( );
					}

					std::string name( ) const override {
						return "std::string";
					}

					std::string json_name( std::string member_name ) const override {
						return "json_string<" + member_name + ">";
					}

					type_info_t *clone( ) const override {
						return new ti_string( *this );
					}
				};

				struct ti_object : public type_info_t {
					std::string object_name;

					size_t type( ) const override {
						return daw::json::json_value_t::index_of<
						  daw::json::json_value_t::object_t>( );
					}

					std::string name( ) const override {
						return object_name;
					}
					ti_object( std::string obj_name )
					  : type_info_t{}
					  , object_name{std::move( obj_name )} {}

					type_info_t *clone( ) const override {
						return new ti_object( *this );
					}

					std::string json_name( std::string member_name ) const override {
						return "json_class<" + member_name + ", " + name( ) + ">";
					}
				};

				std::string make_json_type( std::string name, size_t id ) {
					switch( id ) {
					case json::json_value_t::index_of<json::json_value_t::string_t>( ):
						return "json_string<" + name;
					case json::json_value_t::index_of<json::json_value_t::boolean_t>( ):
						return "json_bool<" + name;
					case json::json_value_t::index_of<json::json_value_t::integer_t>( ):
						return "json_number<" + name + ", intmax_t";
					case json::json_value_t::index_of<json::json_value_t::real_t>( ):
						return "json_number<" + name;
					case json::json_value_t::index_of<json::json_value_t::null_t>( ):
						return "json_custom<" + name;
					case json::json_value_t::index_of<json::json_value_t::array_t>( ):
						return "json_array<" + name;
					case json::json_value_t::index_of<json::json_value_t::object_t>( ):
						return "json_class<" + name;
					}
					std::terminate( );
				}

				struct ti_array : public type_info_t {
					size_t type( ) const override {
						return daw::json::json_value_t::index_of<
						  daw::json::json_value_t::array_t>( );
					}

					std::string name( ) const override {
						return "std::vector<" + children.begin( )->second.name( ) + ">";
					}

					std::string json_name( std::string member_name ) const override {
						return "json_array<" + member_name + ", " + name( ) + "," + +">";
					}

					type_info_t *clone( ) const override {
						return new ti_array( *this );
					}
				};

			} // namespace types

			std::vector<types::ti_object>::iterator
			find_by_name( std::vector<types::ti_object> &obj_info,
			              daw::string_view name ) {
				return std::find_if( obj_info.begin( ), obj_info.end( ),
				                     [n = name.to_string( )]( auto const &item ) {
					                     return n == item.name( );
				                     } );
			}

			void add_or_merge( std::vector<types::ti_object> &obj_info,
			                   types::ti_object const &obj ) {
				auto pos =
				  find_by_name( obj_info, {obj.name( ).data( ), obj.name( ).size( )} );
				if( obj_info.end( ) == pos ) {
					// First time
					obj_info.push_back( obj );
					return;
				}

				types::ti_object &orig = *pos;
				std::vector<std::pair<std::string, types::ti_value>> diff{};

				static auto const comp = []( auto const &c1, auto const &c2 ) {
					return c1.first < c2.first;
				};
				std::set_difference( orig.children.begin( ), orig.children.end( ),
				                     obj.children.begin( ), obj.children.end( ),
				                     std::back_inserter( diff ), comp );

				for( auto &child : diff ) {
					if( child.second.is_null( ) ) {
						if( !orig.children[child.first].is_null( ) ) {
							orig.children[child.first] = child.second;
						}
						orig.children[child.first].is_optional( ) = true;
					} else if( orig.children[child.first].is_null( ) ) {
						orig.children[child.first] = child.second;
					} /* else {
					   orig.children[child.first] = child.second;
					 }*/
					orig.children[child.first].is_optional( ) = true;
				}
			}

			types::ti_value merge_array_values( types::ti_value const &a,
			                                    types::ti_value const &b ) {
				using daw::json::json_value_t;
				types::ti_value result{};
				if( a.is_null( ) ) {
					result = b;
					result.is_optional( ) = true;
				} else if( b.is_null( ) ) {
					result = a;
					result.is_optional( ) = true;
				} else {
					result = a;
					result.is_optional( ) = a.is_optional( ) or b.is_optional( );
				}
				return result;
			}

			types::ti_value parse_json_object(
			  daw::json::json_value_t const &current_item, daw::string_view cur_name,
			  std::vector<types::ti_object> &obj_info, state_t &obj_state ) {

				using daw::json::json_value_t;
				if( current_item.is_integer( ) ) {
					obj_state.has_integrals = true;
					return types::create_ti_value<types::ti_integral>( );
				}
				if( current_item.is_real( ) ) {
					return types::create_ti_value<types::ti_real>( );
				}
				if( current_item.is_boolean( ) ) {
					return types::create_ti_value<types::ti_boolean>( );
				}
				if( current_item.is_string( ) ) {
					obj_state.has_strings = true;
					return types::create_ti_value<types::ti_string>( );
				}
				if( current_item.is_null( ) ) {
					obj_state.has_optionals = true;
					return types::create_ti_value<types::ti_null>( );
				}
				if( current_item.is_object( ) ) {
					auto result = types::ti_object{cur_name.to_string( ) + "_t"};
					for( auto const &child : current_item.get_object( ) ) {
						std::string const child_name =
						  replace_keywords( child.first.to_string( ) );
						result.children[child_name] = parse_json_object(
						  child.second, child_name, obj_info, obj_state );
					}
					add_or_merge( obj_info, result );
					return result;
				}
				if( current_item.is_array( ) ) {
					obj_state.has_arrays = true;
					auto result = types::create_ti_value<types::ti_array>( );
					auto arry = current_item.get_array( );
					auto const child_name = cur_name.to_string( ) + "_element";
					if( arry.empty( ) ) {
						result.children( )[child_name] =
						  types::create_ti_value<types::ti_null>( );
					} else {
						auto const last_item = arry.back( );
						arry.pop_back( );
						auto child =
						  parse_json_object( last_item, child_name, obj_info, obj_state );
						for( auto const &element : current_item.get_array( ) ) {
							child = merge_array_values(
							  child,
							  parse_json_object( element, child_name, obj_info, obj_state ) );
						}
						result.children( )[child_name] = child;
					}
					return result;
				}
				std::cerr << "Unexpected exit point to parse_json_object2";
				std::terminate( );
			}

			std::vector<types::ti_object>
			parse_json_object( daw::json::json_value_t const &current_item,
			                   state_t &obj_state ) {
				using namespace daw::json;
				std::vector<types::ti_object> result{};

				if( !current_item.is_object( ) ) {
					auto root_obj_member =
					  daw::json::make_object_value_item( "root_obj", current_item );
					json_object_value root_object;
					root_object.members_v.push_back( std::move( root_obj_member ) );
					json_value_t root_value{std::move( root_object )};
					parse_json_object( root_value, "root_object", result, obj_state );
				} else {
					parse_json_object( current_item, "root_object", result, obj_state );
				}
				return result;
			}

			void generate_json_link_maps( std::integral_constant<int, 1>,
			                              bool definition, config_t &config,
			                              types::ti_object const &cur_obj ) {
				if( !config.enable_jsonlink ) {
					return;
				}
				if( definition ) {
					config.cpp_file( )
					  << "void " << cur_obj.object_name << "::json_link_map( ) {\n";
					for( auto const &child : cur_obj.children ) {
						config.cpp_file( )
						  << "\tlink_" << daw::json::to_string( child.second.type( ) );
						config.cpp_file( )
						  << "( \"" << child.first << "\", " << child.first << " );\n";
					}
					config.cpp_file( ) << "}\n\n";
				} else {
					config.header_file( ) << "\tstatic void json_link_map( );\n";
				}
			}

			void generate_json_link_maps( std::integral_constant<int, 3>,
			                              bool definition, config_t &config,
			                              types::ti_object const &cur_obj ) {
				if( !config.enable_jsonlink ) {
					return;
				}
				if( !definition ) {
					using daw::json::json_value_t;
					config.cpp_file( )
					  << "inline auto describe_json_class( " << cur_obj.object_name
					  << " ) {\n\tusing namespace daw::json;\n";
					for( auto const &child : cur_obj.children ) {
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

					for( auto const &child : cur_obj.children ) {
						config.cpp_file( ) << "\t\t";
						if( !is_first ) {
							config.cpp_file( ) << ",";
						} else {
							is_first = false;
						}
						if( child.second.is_optional( ) ) {
							config.cpp_file( ) << "json_nullable<";
						}
						config.cpp_file( ) << child.second.json_name( child.first );
						if( child.second.is_optional( ) ) {
							config.cpp_file( ) << ">\n";
						} else {
							config.cpp_file( ) << '\n';
						}
					}
					config.cpp_file( ) << ">{};\n}\n\n";

					config.cpp_file( ) << "inline auto to_json_data( "
					                   << cur_obj.object_name << " const & value ) {\n";
					config.cpp_file( ) << "\treturn std::forward_as_tuple( ";
					is_first = true;
					for( auto const &child : cur_obj.children ) {
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

			inline void generate_json_link_maps( bool definition, config_t &config,
			                                     types::ti_object const &cur_obj ) {

				return generate_json_link_maps( std::integral_constant<int, 3>{},
				                                definition, config, cur_obj );
			}

			void generate_includes( bool definition, config_t &config,
			                        state_t const &obj_state ) {
				{
					std::string const header_message =
					  "// Code auto generated from json file '" +
					  config.json_path.string( ) + "'\n\n";
					if( definition ) {
						if( config.separate_files ) {
							config.cpp_file( ) << header_message;
						}
					} else {
						config.header_file( ) << header_message;
					}
				}
				if( definition ) {
					if( config.separate_files ) {
						config.cpp_file( )
						  << "#include " << config.header_path.filename( ) << "\n\n";
					}
				} else {
					if( config.separate_files ) {
						config.header_file( ) << "#pragma once\n\n";
					}
					config.header_file( ) << "#include <tuple>\n";
					if( obj_state.has_optionals )
						config.header_file( ) << "#include <optional>\n";
					if( obj_state.has_integrals )
						config.header_file( ) << "#include <cstdint>\n";
					if( obj_state.has_strings )
						config.header_file( ) << "#include <string>\n";
					if( obj_state.has_arrays )
						config.header_file( ) << "#include <vector>\n";
					if( config.enable_jsonlink )
						config.header_file( ) << "#include <daw/json/daw_json_link.h>\n";
					config.header_file( ) << '\n';
				}
			}

			void generate_declarations( std::vector<types::ti_object> const &obj_info,
			                            config_t &config, state_t const &obj_state ) {
				for( auto const &cur_obj : obj_info ) {
					auto const obj_type = cur_obj.name( );
					config.header_file( ) << "struct " << obj_type << " {\n";
					for( auto const &child : cur_obj.children ) {
						auto const &member_name = child.first;
						auto const &member_type = child.second.name( );
						config.header_file( ) << "\t";
						if( child.second.is_optional( ) ) {
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
			                           config_t &config, state_t const &obj_state ) {
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
				generate_declarations( obj_info, config, obj_state );
				generate_definitions( obj_info, config, obj_state );
			}

		} // namespace

		void generate_cpp( daw::string_view json_string, config_t &config ) {
			state_t obj_state;
			auto json_obj = daw::json::parse_json( json_string );
			auto obj_info = parse_json_object( json_obj, obj_state );
			generate_code( obj_info, config, obj_state );
		}
	} // namespace json_to_cpp
} // namespace daw
