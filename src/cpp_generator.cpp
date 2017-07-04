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
		object_state_t::~object_state_t( ) {}

		object_state_t::object_state_t( )
		    : has_arrays{false}, has_integrals{false}, has_optionals{false}, has_strings{false} {}

		namespace generate {
			namespace {
				void generate_default_constructor( bool definition, daw::json_to_cpp::config_t &config,
				                                   types::ti_object const &cur_obj ) {
					if( !config.enable_jsonlink ) {
						return;
					}
					if( definition ) {
						config.cpp_file( ) << cur_obj.object_name << "::" << cur_obj.object_name << "( ):\n";
						config.cpp_file( ) << "\t\tdaw::json::JsonLink<" << cur_obj.object_name << ">{ }";
						for( auto const &child : cur_obj.children ) {
							config.cpp_file( ) << ",\n\t\t" << child.first << "{ }";
						}
						config.cpp_file( ) << " {\n\n\tset_links( );\n}\n\n";
					} else {
						config.header_file( ) << "\t" << cur_obj.object_name << "( );\n";
					}
				}

				void generate_copy_constructor( bool definition, daw::json_to_cpp::config_t &config,
				                                types::ti_object const &cur_obj ) {
					if( !config.enable_jsonlink ) {
						return;
					}
					if( definition ) {
						config.cpp_file( ) << cur_obj.object_name << "::" << cur_obj.object_name << "( "
						                   << cur_obj.object_name << " const & other ):\n";
						config.cpp_file( ) << "\t\tdaw::json::JsonLink<" << cur_obj.object_name << ">{ }";
						for( auto const &child : cur_obj.children ) {
							config.cpp_file( ) << ",\n\t\t" << child.first << "{ other." << child.first << " }";
						}
						config.cpp_file( ) << " {\n\n\tset_links( );\n}\n\n";
					} else {
						config.header_file( )
						    << "\t" << cur_obj.object_name << "( " << cur_obj.object_name << " const & other );\n";
					}
				}

				void generate_move_constructor( bool definition, daw::json_to_cpp::config_t &config,
				                                types::ti_object const &cur_obj ) {
					if( !config.enable_jsonlink ) {
						return;
					}
					if( definition ) {
						config.cpp_file( ) << cur_obj.object_name << "::" << cur_obj.object_name << "( "
						                   << cur_obj.object_name << " && other ):\n";
						config.cpp_file( ) << "\t\tdaw::json::JsonLink<" << cur_obj.object_name << ">{ }";
						for( auto const &child : cur_obj.children ) {
							config.cpp_file( )
							    << ",\n\t\t" << child.first << "{ std::move( other." << child.first << " ) }";
						}
						config.cpp_file( ) << " {\n\n\tset_links( );\n}\n\n";
					} else {
						config.header_file( )
						    << "\t" << cur_obj.object_name << "( " << cur_obj.object_name << " && other );\n";
					}
				}

				void generate_destructor( bool definition, daw::json_to_cpp::config_t &config,
				                          types::ti_object const &cur_obj ) {
					if( !config.enable_jsonlink ) {
						return;
					}
					if( definition ) {
						config.cpp_file( ) << cur_obj.object_name << "::~" << cur_obj.object_name << "( ) { }\n\n";
					} else {
						config.header_file( ) << "\t~" << cur_obj.object_name << "( );\n";
					}
				}

				void generate_set_links( bool definition, daw::json_to_cpp::config_t &config,
				                         types::ti_object const &cur_obj ) {
					if( !config.enable_jsonlink ) {
						return;
					}
					if( definition ) {
						config.cpp_file( ) << "void " << cur_obj.object_name << "::set_links( ) {\n";
						for( auto const &child : cur_obj.children ) {
							config.cpp_file( ) << "\tlink_" << to_string( child.second.type( ) );
							config.cpp_file( ) << "( \"" << child.first << "\", " << child.first << " );\n";
						}
						config.cpp_file( ) << "}\n\n";
					} else {
						config.header_file( ) << "private:\n";
						config.header_file( ) << "\tvoid set_links( );\n";
					}
				}

				void generate_jsonlink( bool definition, daw::json_to_cpp::config_t &config,
				                        types::ti_object const &cur_obj ) {
					if( !config.enable_jsonlink ) {
						return;
					}
					generate_default_constructor( definition, config, cur_obj );
					generate_copy_constructor( definition, config, cur_obj );
					generate_move_constructor( definition, config, cur_obj );
					generate_destructor( definition, config, cur_obj );
					if( !definition ) {
						auto const obj_type = cur_obj.name( );
						config.header_file( )
						    << "\n\t" << obj_type << " & operator=( " << obj_type << " const & ) = default;\n";
						config.header_file( )
						    << "\t" << obj_type << " & operator=( " << obj_type << " && ) = default;\n";
					}
					generate_set_links( definition, config, cur_obj );
				}

				void generate_includes( bool definition, daw::json_to_cpp::config_t &config,
				                        object_state_t const &obj_state ) {
					{
						std::string const header_message =
						    "// Code auto generated from json file '" + config.json_path.string( ) + "'\n\n";
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
							config.cpp_file( ) << "#include " << config.header_path.filename( ) << "\n\n";
						}
					} else {
						if( config.separate_files ) {
							config.header_file( ) << "#pragma once\n\n";
						}
						if( obj_state.has_optionals )
							config.header_file( ) << "#include <boost/optional.hpp>\n";
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
				                            daw::json_to_cpp::config_t &config, object_state_t const &obj_state ) {
					for( auto const &cur_obj : obj_info ) {
						auto const obj_type = cur_obj.name( );
						config.header_file( ) << "struct " << obj_type;
						if( config.enable_jsonlink ) {
							config.header_file( ) << ": public daw::json::JsonLink<" << obj_type << ">";
						}
						config.header_file( ) << " {\n";
						for( auto const &child : cur_obj.children ) {
							auto const &member_name = child.first;
							auto const &member_type = child.second.name( );
							config.header_file( ) << "\t";
							if( child.second.is_optional( ) ) {
								config.header_file( ) << "boost::optional<" << member_type << ">";
							} else {
								config.header_file( ) << member_type;
							}
							config.header_file( ) << " " << member_name << ";\n";
						}
						if( config.enable_jsonlink ) {
							config.header_file( ) << "\n";
							generate_jsonlink( false, config, cur_obj );
						}
						config.header_file( ) << "};"
						                      << "\t// " << obj_type << "\n\n";
					}
				}

				void generate_definitions( std::vector<types::ti_object> const &obj_info,
				                           daw::json_to_cpp::config_t &config, object_state_t const &obj_state ) {
					if( !config.enable_jsonlink ) {
						return;
					}
					for( auto const &cur_obj : obj_info ) {
						generate_jsonlink( true, config, cur_obj );
					}
				}
			} // namespace

			void generate_code( std::vector<types::ti_object> const &obj_info, daw::json_to_cpp::config_t &config,
			                    object_state_t const &obj_state ) {
				generate_includes( true, config, obj_state );
				generate_includes( false, config, obj_state );
				generate_declarations( obj_info, config, obj_state );
				generate_definitions( obj_info, config, obj_state );
			}
		} // namespace generate
	}     // namespace json_to_cpp
} // namespace daw

