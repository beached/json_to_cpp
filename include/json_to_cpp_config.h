// The MIT License (MIT)
//
// Copyright (c) 2019 Darrell Wright
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

#pragma once

#include <iostream>
#include <string>
#include <vector>

namespace daw::json_to_cpp {
	struct config_t final {
		bool enable_jsonlink = true;
		std::ostream *header_stream = nullptr;
		std::ostream *cpp_stream = nullptr;
		std::string root_object_name;
		std::string type_prefix{};
		std::string type_suffix{};
		std::optional<std::string> path{};
		boost::filesystem::path cpp_path;
		boost::filesystem::path json_path;
		std::vector<std::vector<std::string>> kv_paths;
		bool hide_null_only;
		bool use_string_view;
		bool has_cpp20;

		std::ostream &header_file( );
		std::ostream &cpp_file( );

		inline bool path_matches( std::vector<std::string> const &cur_path ) const {
			for( auto const &kv_path : kv_paths ) {
				if( kv_path == cur_path ) {
					return true;
				}
			}
			return false;
		}
	};
} // namespace daw::json_to_cpp
