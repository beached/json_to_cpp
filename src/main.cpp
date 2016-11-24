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

	boost::optional<std::string> download( boost::string_view url ) {
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
	if( argc != 2 ) {
		std::cerr << "Must supply a url or a file as the json source\n";
		exit( EXIT_FAILURE );
	}
	std::string const uri{ argv[1] };
	std::string json_str;
	if( is_url( uri ) ) {
		auto tmp = download( uri );
		if( tmp ) {
			json_str = *tmp;
		} else {
			std::cerr << "Could not download json data from '" << uri << "'\n";
			exit( EXIT_FAILURE );
		}
	} else {
		std::ifstream in_file;
		in_file.open( argv[1] );
		if( !in_file ) {
			std::cerr << "Must supply valid json file on commandline\n";
			exit( EXIT_FAILURE );
		}
		std::copy( std::istream_iterator<char>{ in_file }, std::istream_iterator<char>{ }, std::back_inserter( json_str ) );
		in_file.close( );
	}
	config_t config{ };
	generate_cpp( json_str, std::cout, config );
	std::cout << std::endl;

	return EXIT_SUCCESS;
}

