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

#include <cstdlib>
#include <fstream>
#include <iostream>

#include "json_to_cpp.h"

int main( int argc, char ** argv ) {
	using namespace daw::json_to_cpp;

	std::ifstream in_file;
	in_file.open( argv[1] );
	if( !in_file ) {
		std::cerr << "Must supply valid json file on commandline\n";
		exit( EXIT_FAILURE );
	}
	std::string json_blob;
	std::copy( std::istream_iterator<char>{ in_file }, std::istream_iterator<char>{ }, std::back_inserter( json_blob ) );
	in_file.close( );
	config_t config{ };
	auto code = generate_cpp( json_blob, config );
	std::cout << code << '\n';

	return EXIT_SUCCESS;
}

