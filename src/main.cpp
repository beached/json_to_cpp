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

#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <memory>
#include <optional>
#include <string>
#include <vector>

#include <daw/daw_string_view.h>

#include "curl_t.h"
#include "json_to_cpp.h"

int main( int argc, char **argv ) {
	static std::string const default_user_agent =
	  "Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) "
	  "Chrome/54.0.2840.100 Safari/537.36";

	boost::program_options::options_description desc{"Options"};
	desc.add_options( )( "help", "print option descriptions" )(
	  "in_file", boost::program_options::value<boost::filesystem::path>( ),
	  "json source file path or url" )(
	  "kv_paths", boost::program_options::value<std::vector<std::string>>( ),
	  "Specify class members that are key value pairs" )(
	  "use_jsonlink",
	  boost::program_options::value<bool>( )->default_value( true ),
	  "Use JsonLink serializaion/deserialization" )(
	  "output_file", boost::program_options::value<boost::filesystem::path>( ),
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
	  "user_agent",
	  boost::program_options::value<std::string>( )->default_value(
	    default_user_agent ),
	  "User agent to use when downloading via URL" );

	auto vm = boost::program_options::variables_map{};
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
	auto config = daw::json_to_cpp::config_t{};

	if( !vm.count( "in_file" ) ) {
		std::cerr << "Missing in_file parameter\n";
		exit( EXIT_FAILURE );
	}
	config.json_path = vm["in_file"].as<boost::filesystem::path>( );

	if( vm.count( "kv_paths" ) > 0 ) {
		config.kv_paths = vm["kv_paths"].as<std::vector<std::string>>( );
	}

	auto json_str = std::string{};
	if( daw::curl::is_url( config.json_path.string( ) ) ) {
		auto tmp = daw::curl::download( config.json_path.string( ),
		                                vm["user_agent"].as<std::string>( ) );
		if( !tmp ) {
			std::cerr << "Could not download json data from '"
			          << canonical( config.json_path ) << "'\n";
			exit( EXIT_FAILURE );
		}
		json_str = *tmp;
	} else {
		if( !exists( config.json_path ) ) {
			std::cerr << "Could not file file '" << config.json_path << "'\n";
			std::cerr << "Command line options\n" << desc << std::endl;
			exit( EXIT_FAILURE );
		}

		auto in_file = std::ifstream{config.json_path.string( )};
		if( !in_file ) {
			std::cerr << "Could not open json in_file '"
			          << canonical( config.json_path ) << "'\n";
			exit( EXIT_FAILURE );
		}
		std::copy( std::istream_iterator<char>{in_file},
		           std::istream_iterator<char>{}, std::back_inserter( json_str ) );
		in_file.close( );
	}

	config.cpp_stream = &std::cout;
	config.header_stream = &std::cout;
	config.enable_jsonlink = vm["use_jsonlink"].as<bool>( );
	config.hide_null_only = vm["hide_null_only"].as<bool>( );
	config.use_string_view = vm["use_string_view"].as<bool>( );
	auto cpp_file = std::ofstream{};
	auto header_file = std::ofstream{};

	if( vm.count( "cpp_file" ) > 0 ) {
		bool const allow_overwrite = vm["allow_overwrite"].as<bool>( );
		config.cpp_path =
		  canonical( vm["output_file"].as<boost::filesystem::path>( ) );
		if( exists( config.cpp_path ) && !allow_overwrite ) {
			std::cerr << "output_file '" << config.cpp_path << "' already exists\n";
			exit( EXIT_FAILURE );
		}
		cpp_file.open( config.cpp_path.string( ), std::ios::out | std::ios::trunc );
		if( !cpp_file ) {
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
