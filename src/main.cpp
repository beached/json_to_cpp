// The MIT License ( MIT )
//
// Copyright ( c ) 2016 Darrell Wright
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

#include <boost/algorithm/string/predicate.hpp>
#include <boost/optional.hpp>
#include <boost/program_options.hpp>
#include <boost/utility/string_view.hpp>
#include <cstdlib>
#include <curl/curl.h>
#include <fstream>
#include <string>
#include <memory>
#include <iostream>

#include "json_to_cpp.h"
namespace {
	size_t callback( char const * in, size_t const size, size_t const num, std::string * const out ) {
		assert( out );
		size_t totalBytes = size * num;
		out->append( in, totalBytes );
		return totalBytes;
	}

	boost::optional<std::string> download( boost::string_view url, boost::string_view user_agent ) {
		struct curl_slist *headers = nullptr;
		curl_slist_append( headers, "Accept: application/json" );  
		curl_slist_append( headers, "Content-Type: application/json" );
		curl_slist_append( headers, "charsets: utf-8" ); 

		CURL* curl = curl_easy_init( );
		if( !curl ) {
			return boost::none;
		}
		curl_easy_setopt( curl, CURLOPT_HTTPHEADER, headers );
		curl_easy_setopt( curl, CURLOPT_HTTPGET, 1 ); 
		curl_easy_setopt( curl, CURLOPT_HTTPHEADER, headers); 
		curl_easy_setopt(curl, CURLOPT_USERAGENT, user_agent.data( ) );
		  
		// Set remote URL.
		curl_easy_setopt( curl, CURLOPT_URL, url.data( ) );

		// Don't wait forever, time out after 10 seconds.
		curl_easy_setopt( curl, CURLOPT_TIMEOUT, 15 );

		// Follow HTTP redirects if necessary.
		curl_easy_setopt( curl, CURLOPT_FOLLOWLOCATION, 1L );

		// Response information.
		int httpCode = 0;
		std::string httpData;

		// Hook up data handling function.
		curl_easy_setopt( curl, CURLOPT_WRITEFUNCTION, callback );

		// Hook up data container ( will be passed as the last parameter to the
		// callback handling function ).  Can be any pointer type, since it will
		// internally be passed as a void pointer.
		curl_easy_setopt( curl, CURLOPT_WRITEDATA, &httpData );

		// Run our HTTP GET command, capture the HTTP response code, and clean up.
		curl_easy_perform( curl );
		curl_easy_getinfo( curl, CURLINFO_RESPONSE_CODE, &httpCode );
		curl_easy_cleanup( curl );

		if( httpCode != 200 ) {
			std::cerr << "Couldn't GET from " << url << '\n';
			return boost::none;
		}

		return httpData;
	}

	bool is_url( boost::string_view path ) {
		return boost::starts_with( path.data( ), "http://" ) || boost::starts_with( path.data( ), "https://" );
	}
}	// namespace anonymous

int main( int argc, char ** argv ) {
	using namespace daw::json_to_cpp;

	boost::program_options::options_description desc{ "Options" };
	desc.add_options( )
		( "help", "print option descriptions" )
		( "in_file", boost::program_options::value<std::string>( ), "json source file path or url" )
		( "out_file", boost::program_options::value<std::string>( ), "output c++ file" )
		( "use_jsonlink", boost::program_options::value<bool>( )->default_value( true ), "Use JsonLink serializaion/deserialization" )
		( "user_agent", boost::program_options::value<std::string>( )->default_value( "Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/54.0.2840.100 Safari/537.36" ), "User agent to use when downloading via URL" );

	boost::program_options::variables_map vm;
	try {
		boost::program_options::store( boost::program_options::parse_command_line( argc, argv, desc ), vm );
		
		if( vm.count( "help" ) ) {
			std::cout << "Command line options\n" << desc << std::endl;
			return EXIT_SUCCESS;
		}
		boost::program_options::notify( vm );
	} catch( boost::program_options::error const & po_error ) {
		std::cerr << "ERROR: " << po_error.what( ) << '\n';
		std::cerr << desc << std::endl;
		return EXIT_FAILURE;
	}

	if( !vm.count( "in_file" ) ) {
		std::cout << "Missing in_file parameter\n";
		std::cout << "Command line options\n" << desc << std::endl;
		exit( EXIT_FAILURE );
	}
	auto uri = vm["in_file"].as<std::string>( );
	std::string json_str;
	if( is_url( uri ) ) {
		auto tmp = download( uri, vm["user_agent"].as<std::string>( ) );
		if( tmp ) {
			json_str = *tmp;
		} else {
			std::cerr << "Could not download json data from '" << uri << "'\n";
			exit( EXIT_FAILURE );
		}
	} else {
		std::ifstream in_file;
		in_file.open( uri );
		if( !in_file ) {
			std::cerr << "Could not open json in_file '" << uri << "'\n";
			exit( EXIT_FAILURE );
		}
		std::copy( std::istream_iterator<char>{ in_file }, std::istream_iterator<char>{ }, std::back_inserter( json_str ) );
		in_file.close( );
	}
	config_t config{ };
	config.enable_jsonlink = vm["use_jsonlink"].as<bool>( );

	if( vm.count( "out_file" ) ) {
		std::ofstream out_file;
		auto const out_filename = vm["out_file"].as<std::string>( );
		out_file.open( out_filename, std::ios::out | std::ios::trunc );
		if( !out_file ) {
			std::cerr << "Could not open cpp out_file '" << out_filename << "'\n";
			exit( EXIT_FAILURE );
		}
		generate_cpp( json_str, out_file, config );	
	} else {
		generate_cpp( json_str, std::cout, config );
		std::cout << std::endl;
	}

	return EXIT_SUCCESS;
}

