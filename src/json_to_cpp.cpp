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


#include <algorithm>
#include <boost/utility/string_view.hpp>
#include <limits>
#include <ostream>
#include <string>
#include <tuple>
#include <unordered_map>

#include <daw/json/daw_json_link.h>

#include "json_to_cpp.h"

namespace daw {
	namespace json_to_cpp {
		config_t::config_t( ):
				enable_comments{ true },
				enable_jsonlink{ true } { }

		config_t::~config_t( ) { }
		struct state_t {
			bool has_arrays;
			bool has_integrals;
			bool has_optionals;
			bool has_strings;
			state_t( ):
				has_arrays{ false },
				has_integrals{ false },
				has_optionals{ false },
				has_strings{ false } { }
		};

		namespace {
			namespace types {
				struct type_info_t;

				struct ti_value {
					type_info_t * value;

					std::string name( ) const noexcept;
					std::unordered_map<std::string, ti_value> const & children( ) const;
					std::unordered_map<std::string, ti_value> & children( );
					bool & is_optional( ) noexcept;
					bool const & is_optional( ) const noexcept;
					daw::json::impl::value_t::value_types type( ) const;	

					ti_value( ): value{ nullptr } { }
					~ti_value( );
					ti_value( ti_value const & other );
					ti_value( ti_value && other ): value{ std::exchange( other.value, nullptr ) } { }

					ti_value & operator=( ti_value const & rhs ) {
						if( this != &rhs ) {
							ti_value tmp{ rhs };
							using std::swap;
							swap( *this, tmp );
						}
						return *this;
					}

					ti_value & operator=( ti_value && rhs ) {
						if( this != &rhs ) {
							value = std::exchange( rhs.value, nullptr );
						}
						return *this;
					}

					template<typename Derived>
					ti_value( Derived other ): value{ new Derived( std::move( other ) ) } { }

					template<typename Derived>
					ti_value & operator=( Derived rhs ) {
						value = new Derived( std::move( rhs ) );
						return *this;
					}
				};

				template<typename Derived, typename... Args>
				ti_value create_ti_value( Args&&... args ) {
					ti_value result;
					auto tmp = new Derived( std::forward<Args>( args )... );
					result.value = std::exchange( tmp, nullptr );
					return result;
				}

				struct type_info_t {
					bool is_optional;
					std::unordered_map<std::string, ti_value> children;

					type_info_t( ): is_optional{ false }, children{ } { }
					type_info_t( type_info_t const & ) = default;
					type_info_t( type_info_t && ) = default;
					type_info_t & operator=( type_info_t const & ) = default;
					type_info_t & operator=( type_info_t && ) = default;
					virtual ~type_info_t( );

					virtual daw::json::impl::value_t::value_types type( ) const = 0;
					virtual std::string name( ) const = 0;

					virtual type_info_t * clone( ) const = 0;
				};	// type_info_t

				type_info_t::~type_info_t( ) { }

				std::string ti_value::name( ) const noexcept {
					return value->name( );
				}

				daw::json::impl::value_t::value_types ti_value::type( ) const {
					return value->type( );
				}
				std::unordered_map<std::string, ti_value> const & ti_value::children( ) const {
					return value->children;
				}

				std::unordered_map<std::string, ti_value> & ti_value::children( ) {
					return value->children;
				}

				bool & ti_value::is_optional( ) noexcept {
					return value->is_optional;
				}

				bool const & ti_value::is_optional( ) const noexcept {
					return value->is_optional;
				}

				struct ti_null: public type_info_t {
					daw::json::impl::value_t::value_types type( ) const override {
						return daw::json::impl::value_t::value_types::null;
					}

					std::string name( ) const override {
						return "void*";
					}

					ti_null( ): type_info_t{ } {
						is_optional = true;
					}

					type_info_t * clone( ) const override {
						return new ti_null( *this );
					}
				};

				ti_value::ti_value( ti_value const & other ): value{ other.value->clone( ) } { }

				ti_value::~ti_value( ) {
					if( nullptr != value ) {
						auto tmp = value;
						value = nullptr;
						delete tmp;
					}
				}

				struct ti_integral: public type_info_t {
					daw::json::impl::value_t::value_types type( ) const override {
						return daw::json::impl::value_t::value_types::integral;
					}

					std::string name( ) const override {
						return "int64_t";
					}

					type_info_t * clone( ) const override {
						return new ti_integral( *this );
					}
				};

				struct ti_real: public type_info_t {
					daw::json::impl::value_t::value_types type( ) const override {
						return daw::json::impl::value_t::value_types::real;
					}

					std::string name( ) const override {
						return "double";
					}

					type_info_t * clone( ) const override {
						return new ti_real( *this );
					}
				};

				struct ti_boolean: public type_info_t {
					daw::json::impl::value_t::value_types type( ) const override {
						return daw::json::impl::value_t::value_types::boolean;
					}

					std::string name( ) const override {
						return "bool";
					}

					type_info_t * clone( ) const override {
						return new ti_boolean( *this );
					}
				};

				struct ti_string: public type_info_t {
					daw::json::impl::value_t::value_types type( ) const override {
						return daw::json::impl::value_t::value_types::string;
					}

					std::string name( ) const override {
						return "std::string";
					}

					type_info_t * clone( ) const override {
						return new ti_string( *this );
					}
				};

				struct ti_object: public type_info_t {
					std::string object_name;

					daw::json::impl::value_t::value_types type( ) const override {
						return daw::json::impl::value_t::value_types::object;
					}

					std::string name( ) const override {
						return object_name;
					}
					ti_object( std::string obj_name ):
						type_info_t{ },
						object_name{ std::move( obj_name ) } { }

					type_info_t * clone( ) const override {
						return new ti_object( *this );
					}
				};

				struct ti_array: public type_info_t { 
					daw::json::impl::value_t::value_types type( ) const override {
						return daw::json::impl::value_t::value_types::array;
					}

					std::string name( ) const override {
						return "std::vector<" + children.begin( )->second.name( ) + ">";
					}

					type_info_t * clone( ) const override {
						return new ti_array( *this );
					}
				};

			}

			std::vector<types::ti_object>::iterator find_by_name( std::vector<types::ti_object> & obj_info, boost::string_view name ) {
				return std::find_if( obj_info.begin( ), obj_info.end( ), [n=name.to_string( )]( auto const & item ) {
						return n == item.name( );
				} );
			}

			void add_or_merge( std::vector<types::ti_object> & obj_info, types::ti_object const & obj ) {
				auto pos = find_by_name( obj_info, obj.name( ) );
				if( obj_info.end( ) == pos ) {
					obj_info.push_back( obj );
					return;
				}
				for( auto const & child: obj.children ) {
					// Do not overwrite if type is null.  If the destination is already null it doesn't matter
					types::ti_object & child_obj = *pos;
					if( !(daw::json::impl::value_t::value_types::null == child.second.type( ) && pos->children.count( child.first ) > 0) ) {
						child_obj.children[child.first] = child.second;
					} else {
						child_obj.children[child.first].is_optional( ) = true;
					}
				}
			}

			types::ti_value merge_array_values( types::ti_value const & a, types::ti_value const & b) {
				using daw::json::impl::value_t;
				types::ti_value result = b;
				if( a.type( ) == value_t::value_types::null ) {
					result = b;
					result.is_optional( ) = true;
				} else if( b.type( ) == value_t::value_types::null ) {
					result.is_optional( ) = true;
				}
				return result;
			}

			types::ti_value parse_json_object( daw::json::impl::value_t const & current_item, boost::string_view cur_name, std::vector<types::ti_object> & obj_info,  state_t & obj_state ) {
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
						auto result = types::ti_object{ cur_name.to_string( ) + "_t" };
						for( auto const & child: current_item.get_object( ) ) {
							std::string const child_name = child.first.to_string( );
							result.children[child_name] = parse_json_object( child.second, child_name, obj_info, obj_state );
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
							result.children( )[child_name]  = types::create_ti_value<types::ti_null>( ); 
						} else {
							auto const last_item = arry.back( );
							arry.pop_back( );
							auto children = parse_json_object( last_item, child_name, obj_info, obj_state );
							for( auto const & element: current_item.get_array( ) ) {
								children = merge_array_values( children, parse_json_object( element, child_name, obj_info, obj_state ) );
							}
							result.children( )[child_name] = children;
						}
						return result;
					}
				}
				throw std::runtime_error( "Unexpected exit point to parse_json_object2" );
			}

			std::vector<types::ti_object> parse_json_object( daw::json::impl::value_t const & current_item, state_t & obj_state ) {
				using namespace daw::json::impl;
				std::vector<types::ti_object> result;

				if( !current_item.is_object( ) ) {
					auto root_obj_member = daw::json::impl::make_object_value_item( "root_obj", current_item );
					object_value root_object;
					root_object.members_v.push_back( std::move( root_obj_member ) );
					value_t root_value{ std::move( root_object ) };
					parse_json_object( root_value, "root_object", result, obj_state );
				} else {
					parse_json_object( current_item, "root_object", result, obj_state );
				}
				return result;
			}

			void generate_default_constructor( bool definition, std::ostream & ss, types::ti_object const & cur_obj ) {
				if( definition ) {
					ss << cur_obj.object_name << "::" << cur_obj.object_name << "( ):\n";
					ss << "\t\tdaw::json::JsonLink<" << cur_obj.object_name << ">{ }";
					for( auto const & child: cur_obj.children ) {
						ss << ",\n\t\t" << child.first << "{ }";
					}
					ss << " {\n\n\tset_links( );\n}\n\n";
				} else {
					ss << "\t" << cur_obj.object_name << "( );\n";
				}
			}

			void generate_copy_constructor( bool definition, std::ostream & ss, types::ti_object const & cur_obj ) {
				if( definition ) {
					ss << cur_obj.object_name << "::" << cur_obj.object_name << "( " << cur_obj.object_name << " const & other ):\n";
					ss << "\t\tdaw::json::JsonLink<" << cur_obj.object_name << ">{ }";
					for( auto const & child: cur_obj.children ) {
						ss << ",\n\t\t" << child.first << "{ other." << child.first << " }";
					}
					ss << " {\n\n\tset_links( );\n}\n\n";
				} else {
					ss << "\t" << cur_obj.object_name << "( " << cur_obj.object_name << " const & other );\n";
				}
			}

			void generate_move_constructor( bool definition, std::ostream & ss, types::ti_object const & cur_obj ) {
				if( definition ) {
					ss << cur_obj.object_name << "::" << cur_obj.object_name << "( " << cur_obj.object_name << " && other ):\n";
					ss << "\t\tdaw::json::JsonLink<" << cur_obj.object_name << ">{ }";
					for( auto const & child: cur_obj.children ) {
						ss << ",\n\t\t" << child.first << "{ std::move( other." << child.first << " ) }";
					}
					ss << " {\n\n\tset_links( );\n}\n\n";
				} else {
					ss << "\t" << cur_obj.object_name << "( " << cur_obj.object_name << " && other );\n";
				}
			}

			void generate_destructor( bool definition, std::ostream & ss, types::ti_object const & cur_obj ) {
				if( definition ) {
					ss << cur_obj.object_name << "::~" << cur_obj.object_name << "( ) { }\n\n";
				} else {
					ss << "\t~" << cur_obj.object_name << "( );\n";
				}
			}

			void generate_set_links( bool definition, std::ostream & ss, types::ti_object const & cur_obj ) {
				if( definition ) {
					ss << "void " << cur_obj.object_name << "::set_links( ) {\n";
					for( auto const & child: cur_obj.children ) {
						ss << "\tlink_" << to_string( child.second.type( ) );
						ss << "( \"" << child.first << "\", " << child.first << " );\n";
					}
					ss << "}\n\n";
				} else {
					ss << "private:\n";
					ss << "\tvoid set_links( );\n";
				}
			}

			void generate_jsonlink( bool definition, std::ostream & ss, types::ti_object const & cur_obj ) {
				generate_default_constructor( definition, ss, cur_obj );
				generate_copy_constructor( definition, ss, cur_obj );
				generate_move_constructor( definition, ss, cur_obj );
				generate_destructor( definition, ss, cur_obj );
				if( !definition ) {
					auto const obj_type = cur_obj.name( );
					ss << "\n\t" << obj_type << " & operator=( " << obj_type << " const & ) = default;\n";
					ss << "\t" << obj_type << " & operator=( " << obj_type << " && ) = default;\n";
				}
				generate_set_links( definition, ss, cur_obj );
			}


			void generate_includes( std::ostream & ss, config_t const & config, state_t const & obj_state ) {
				if( obj_state.has_optionals ) ss << "#include <boost/optional.hpp>\n";
				if( obj_state.has_integrals ) ss << "#include <cstdint>\n";
				if( obj_state.has_strings ) ss << "#include <string>\n";
				if( obj_state.has_arrays ) ss << "#include <vector>\n";
				if( config.enable_jsonlink ) ss << "#include <daw/json/daw_json_link.h>\n";
				ss << '\n';
			}

			void generate_declarations( std::vector<types::ti_object> const & obj_info, std::ostream & ss, config_t const & config, state_t const & obj_state ) {
				for( auto const & cur_obj: obj_info ) {
					auto const obj_type = cur_obj.name( );
					ss << "struct " << obj_type;
					if( config.enable_jsonlink ) {
						ss << ": public daw::json::JsonLink<" << obj_type << ">";
					}
					ss << " {\n";
					for( auto const & child: cur_obj.children ) {
						auto const & member_name = child.first;
						auto const & member_type = child.second.name( );
						ss << "\t";
						if( child.second.is_optional( ) ) {
							ss << "boost::optional<" << member_type << ">";
						} else {
							ss << member_type;
						}
						ss << " " << member_name << ";\n";
					}
					ss << "\n";
					if( config.enable_jsonlink ) {
						generate_jsonlink( false, ss, cur_obj );
					}
					ss << "};" << "\t// " << obj_type << "\n\n";
				}
			}

			void generate_definitions( std::vector<types::ti_object> const & obj_info, std::ostream & ss, config_t const & config, state_t const & obj_state ) {
				for( auto const & cur_obj: obj_info ) {
					generate_jsonlink( true, ss, cur_obj );
				}
			}

			void generate_code( std::vector<types::ti_object> const & obj_info, std::ostream & ss, config_t const & config, state_t const & obj_state ) {
				ss << "// Code auto generated from json file.\n";
				generate_includes( ss, config, obj_state );
				generate_declarations( obj_info, ss, config, obj_state );
				if( config.enable_jsonlink ) {
					generate_definitions( obj_info, ss, config, obj_state );
				}
			}

		}	// namespace anonymous

		void generate_cpp( boost::string_view json_string, std::ostream & ss, config_t const & config ) {
			state_t obj_state;
			auto json_obj = daw::json::parse_json( json_string );
			auto obj_info = parse_json_object( json_obj, obj_state );
			generate_code( obj_info, ss, config, obj_state );
		}
	}	// namespace json_to_cpp
}    // namespace daw

