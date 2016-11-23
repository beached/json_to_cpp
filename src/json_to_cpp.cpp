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
#include <unordered_map>

#include <daw/json/daw_json_link.h>

#include "json_to_cpp.h"

namespace daw {
	namespace json_to_cpp {
		bool enable_comments;
		bool enable_jsonlink;
		
		void config_t::set_links( ) {
			link_boolean( "enable_comments", enable_comments );
			link_boolean( "enable_jsonlink", enable_jsonlink );
		}

		config_t::config_t( ):
				daw::json::JsonLink<config_t>{ },
				enable_comments{ true },
				enable_jsonlink{ true } {
			
			set_links( );	
		}

		config_t::~config_t( ) { }

		config_t::config_t( config_t const & other ):
				daw::json::JsonLink<config_t>{ },
				enable_comments{ other.enable_comments },
				enable_jsonlink{ other.enable_jsonlink } {
			
			set_links( );	
		}

		config_t::config_t( config_t && other ):
				daw::json::JsonLink<config_t>{ },
				enable_comments{ std::move( other.enable_comments ) },
				enable_jsonlink{ std::move( other.enable_jsonlink ) } {
			
			set_links( );	
		}

		namespace {
			struct val_info_t {
				std::string name;
				bool is_optional;
				daw::json::impl::value_t::value_types type;

				val_info_t( ):
					name{ },
					is_optional{ false },
					type{ daw::json::impl::value_t::value_types::null } { }

			};	// val_info_t;


			struct obj_info_t {
				std::string name;
				std::map<std::string, val_info_t> members;
			};	// obj_info_t


			void obj_to_string( boost::string_view cur_name, daw::json::impl::object_value const & cur_item, std::unordered_map<std::string, obj_info_t> & obj_info ) {
				using daw::json::impl::value_t;
				
				obj_info_t cur_obj;
				cur_obj.name = cur_name.to_string( );
				if( cur_obj.name.empty( ) ) {
					static size_t unknown_count = 0;
					cur_obj.name = "unknown_" + std::to_string( unknown_count++ );
				}
				std::vector<std::string> sub_objects;
				std::vector<std::string> sub_arrays;
				for( auto const & member: cur_item.container( ) ) {
					val_info_t val_info;
					val_info.name = member.first.to_string( );
					val_info.type = member.second.type( );
					cur_obj.members[val_info.name] = val_info;

					if( val_info.type == value_t::value_types::object ) {
						obj_to_string( val_info.name, member.second.get_object( ), obj_info );
					} else if( val_info.type == value_t::value_types::array ) {
						sub_arrays.push_back( val_info.name );
					}

				}
				// This will merge or create
				obj_info[cur_obj.name].name = cur_obj.name;
				for( auto const & member: cur_obj.members ) {
					obj_info[cur_obj.name].members[member.first] = member.second;
				}
			}
				
			std::unordered_map<std::string, obj_info_t> obj_to_string( daw::json::impl::value_t const & json_obj ) {
				std::unordered_map<std::string, obj_info_t> result;
				obj_to_string( "root_type", json_obj.get_object( ), result );
				return result;
			}

			std::string type_to_jsonstring( daw::json::impl::value_t::value_types type ) noexcept {
				using daw::json::impl::value_t;
				switch( type ) {
					case value_t::value_types::integral:
						return "integral";
					case value_t::value_types::real:
						return "real";
					case value_t::value_types::boolean:
						return "boolean";
					case value_t::value_types::string:
						return "string";
					case value_t::value_types::array:
						return "array";
					case value_t::value_types::object:
						return "object";
					case value_t::value_types::null:
						return "null";
				}
				std::abort( );
			}

			std::string type_to_string( boost::string_view name, daw::json::impl::value_t::value_types type ) noexcept {
				using daw::json::impl::value_t;
				switch( type ) {
					case value_t::value_types::integral:
						return "int64_t";
					case value_t::value_types::real:
						return "double";
					case value_t::value_types::boolean:
						return "bool";
					case value_t::value_types::string:
						return "std::string";
					case value_t::value_types::array:
						return "std::vector<void*>";
					case value_t::value_types::object:
						return name.to_string( ) + "_t";
					case value_t::value_types::null:
						return "void *";
				}
				std::abort( );
			}

			std::string generate_code( std::unordered_map<std::string, obj_info_t> obj_info, config_t const & config ) {
				using daw::json::impl::value_t;
				std::stringstream ss;
				for( auto const & obj_info_item: obj_info ) {
					auto const & cur_obj = obj_info_item.second;
					auto const obj_type = cur_obj.name + "_t";
			
					ss << "struct " << cur_obj.name;
					if( config.enable_jsonlink ) {
						ss << ": public daw::json::JsonLink<" << obj_type << ">";
					}
					ss << " {\n";
					for( auto const & item: cur_obj.members ) {
						auto const & member = item.second;
						auto const member_type = type_to_string( member.name, member.type );
						ss << "\t";
						if( member.is_optional ) {
							ss << "boost::optional<" << member_type << ">";
						} else {
							ss << member_type;
						}
						ss << " " << member.name << ";\n\n";
					}
					if( config.enable_jsonlink ) {
						// Default Constructor
						{
							ss << "\t" << obj_type << "( ):\n";
							ss << "\t\t\tdaw::json::JsonLink<" << obj_type << ">{ }";
							for( auto const & member: cur_obj.members ) {
								ss << ",\n\t\t\t" << member.first << "{ }";
							}
							ss << " {\n\t\tset_links( );\n\t}\n";
						}
						// Copy Constructor
						{
							ss << "\t" << obj_type << "( " << obj_type << " const & other ):\n";
							ss << "\t\t\tdaw::json::JsonLink<" << obj_type << ">{ }";
							for( auto const & member: cur_obj.members ) {
								ss << ",\n\t\t\t" << member.first << "{ other." << member.first << " }";
							}
							ss << " {\n\t\tset_links( );\n\t}\n";
						}
						// Move Constructor
						{
							ss << "\t" << obj_type << "( " << obj_type << " && other ):\n";
							ss << "\t\t\tdaw::json::JsonLink<" << obj_type << ">{ }";
							for( auto const & member: cur_obj.members ) {
								ss << ",\n\t\t\t" << member.first << "{ std::move( other." << member.first << " ) }";
							}
							ss << " {\n\t\tset_links( );\n\t}\n";
						}
						// Destructor
						ss << "\t~" << obj_type << "( ) { }\n";
						// copy/move operators
						ss << "\t" << obj_type << " & operator=( " << obj_type << " const & ) = default\n";
						ss << "\t" << obj_type << " & operator=( " << obj_type << " && ) = default\n";
						//	set_links
						ss << "\tvoid set_links( ) {";
						for( auto const & item: cur_obj.members ) {
							auto const & member = item.second;
							ss << "\n\t\tlink_" << type_to_jsonstring( member.type ) << "( \"" << member.name << "\", " << member.name << " );";
						}
						ss << "\n\t}\n";
					}

					ss << "};";
					if( config.enable_comments ) {
						ss << "\t// " << obj_type;
					}
					ss << "\n\n";

				}
				return ss.str( );
			}

		}	// namespace anonymous

		std::string generate_cpp( boost::string_view json_string, config_t const & config ) {
			auto json_obj = daw::json::parse_json( json_string );
			auto obj_info = obj_to_string( json_obj );
			std::string result = generate_code( obj_info, config );

			return result;
				
		}
	}	// namespace json_to_cpp
}    // namespace daw

