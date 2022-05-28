// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, version 1.0. (see accompanying
// file license or copy at http://www.boost.org/license_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#include <boost/program_options.hpp>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <optional>
#include <string>
#include <vector>

#include <daw/daw_string_view.h>

#include "curl_t.h"
#include "json_to_cpp.h"

namespace {
	// Paths are specified with dot separators, if the name has a dot in it,
	// it must be escaped
	// memberA.memberB.member\.C has 3 parts['memberA', 'memberB', 'member.C']
	constexpr daw::string_view pop_json_path( daw::string_view &path ) {
		return path.pop_front_until( [in_escape = false]( char c ) mutable {
			if( in_escape ) {
				in_escape = false;
				return false;
			}
			if( c == '\\' ) {
				in_escape = true;
				return false;
			}
			return ( c == '.' );
		} );
	}

	std::vector<std::vector<std::string>>
	process_paths( std::vector<std::string> const &paths ) {
		auto result = std::vector<std::vector<std::string>>( );
		for( std::string const &p : paths ) {
			if( p.empty( ) ) {
				continue;
			}
			auto cur_item = std::vector<std::string>( );
			auto cur_p = daw::string_view( p.data( ), p.size( ) );
			while( not cur_p.empty( ) ) {
				auto popped_item = pop_json_path( cur_p );
				cur_item.emplace_back( popped_item.begin( ), popped_item.end( ) );
			}
			if( not cur_item.empty( ) ) {
				if( cur_item.front( ) != "root_object" ) {
					cur_item.insert( cur_item.begin( ), "root_object" );
				}
				result.push_back( std::move( cur_item ) );
			}
		}
		return result;
	}
} // namespace

int main( int argc, char **argv ) {
	constexpr daw::string_view default_user_agent =
	  "Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) "
	  "Chrome/54.0.2840.100 Safari/537.36";

	boost::program_options::options_description desc{ "Options" };
	desc.add_options( )( "help", "print option descriptions" )(
	  "in_file", boost::program_options::value<std::filesystem::path>( ),
	  "json source file path or url" )(
	  "kv_paths", boost::program_options::value<std::vector<std::string>>( ),
	  "Specify class members that are key value pairs" )(
	  "use_jsonlink",
	  boost::program_options::value<bool>( )->default_value( true ),
	  "Use JsonLink serializaion/deserialization" )(
	  "has_cpp20", boost::program_options::value<bool>( )->default_value( false ),
	  "Enables use of non-type class template arguments" )(
	  "output_file", boost::program_options::value<std::filesystem::path>( ),
	  "output goes to c++ header file." )(
	  "allow_overwrite",
	  boost::program_options::value<bool>( )->default_value( false ),
	  "Overwrite existing output files" )(
	  "hide_null_only",
	  boost::program_options::value<bool>( )->default_value( true ),
	  "Do not output json entries that are only ever null" )(
	  "use_string_view",
	  boost::program_options::value<bool>( )->default_value( false ),
	  "Use std::string_view instead of std::string.  Must ensure buffer is "
	  "available after parsing when this is used" )(
	  "root_object",
	  boost::program_options::value<std::string>( )->default_value(
	    "root_object" ),
	  "Name of the nameless root object" )(
	  "user_agent",
	  boost::program_options::value<std::string>( )->default_value(
	    static_cast<std::string>( default_user_agent ) ),
	  "User agent to use when downloading via URL" );

	auto vm = boost::program_options::variables_map( );
	try {
		boost::program_options::store(
		  boost::program_options::parse_command_line( argc, argv, desc ), vm );
		if( vm.count( "help" ) ) {
			std::cout << "Command line options\n" << desc << std::endl;
			return EXIT_SUCCESS;
		}
		boost::program_options::notify( vm );
	} catch( boost::program_options::error const &po_error ) {
		std::cerr << "ERROR: " << po_error.what( ) << '\n';
		std::cerr << desc << std::endl;
		return EXIT_FAILURE;
	}
	auto config = daw::json_to_cpp::config_t( );

	if( not vm.count( "in_file" ) ) {
		std::cerr << "Missing in_file parameter\n";
		exit( EXIT_FAILURE );
	}
	config.json_path = vm["in_file"].as<std::filesystem::path>( );
	config.root_object_name = vm["root_object"].as<std::string>( );

	if( vm.count( "kv_paths" ) > 0 ) {
		config.kv_paths =
		  process_paths( vm["kv_paths"].as<std::vector<std::string>>( ) );
	}

	auto const json_str = [&]( ) {
		if( auto const p = config.json_path.string( ); daw::curl::is_url( p ) ) {
			auto tmp = daw::curl::download( p, vm["user_agent"].as<std::string>( ) );
			if( not tmp ) {
				std::cerr << "Could not download json data from '"
				          << canonical( config.json_path ) << "'\n";
				exit( EXIT_FAILURE );
			}
			return *tmp;
		} else {
			if( not exists( config.json_path ) ) {
				std::cerr << "Could not file file '" << config.json_path << "'\n";
				std::cerr << "Command line options\n" << desc << std::endl;
				exit( EXIT_FAILURE );
			}

			auto in_file = std::ifstream( config.json_path.string( ) );
			if( not in_file ) {
				std::cerr << "Could not open json in_file '"
				          << canonical( config.json_path ) << "'\n";
				exit( EXIT_FAILURE );
			}
			auto tmp = std::string( );
			std::copy( std::istream_iterator<char>( in_file ),
			           std::istream_iterator<char>( ), std::back_inserter( tmp ) );
			return tmp;
		}
	}( );

	config.cpp_stream = &std::cout;
	config.header_stream = &std::cout;
	config.enable_jsonlink = vm["use_jsonlink"].as<bool>( );
	config.hide_null_only = vm["hide_null_only"].as<bool>( );
	config.use_string_view = vm["use_string_view"].as<bool>( );
	config.has_cpp20 = vm["has_cpp20"].as<bool>( );
	auto cpp_file = std::ofstream( );
	auto header_file = std::ofstream( );

	if( vm.count( "cpp_file" ) > 0 ) {
		bool const allow_overwrite = vm["allow_overwrite"].as<bool>( );
		config.cpp_path =
		  canonical( vm["output_file"].as<std::filesystem::path>( ) );
		if( exists( config.cpp_path ) and not allow_overwrite ) {
			std::cerr << "output_file '" << config.cpp_path << "' already exists\n";
			exit( EXIT_FAILURE );
		}
		cpp_file.open( config.cpp_path.string( ), std::ios::out | std::ios::trunc );
		if( not cpp_file ) {
			std::cerr << "Could not open cpp_file '" << config.cpp_path
			          << "' for writing\n";
			exit( EXIT_FAILURE );
		}
		config.cpp_stream = &cpp_file;
		config.header_stream = &cpp_file;
	}
	daw::json_to_cpp::generate_cpp( json_str, config );

	return EXIT_SUCCESS;
}
