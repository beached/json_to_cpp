// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, version 1.0. (see accompanying
// file license or copy at http://www.boost.org/license_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include <filesystem>
#include <iostream>
#include <string>
#include <vector>

namespace daw::json_to_cpp {
	struct config_t final {
		bool enable_jsonlink = true;
		std::ostream *header_stream = nullptr;
		std::ostream *cpp_stream = nullptr;
		std::string root_object_name;
		std::string type_prefix{ };
		std::string type_suffix{ };
		std::optional<std::string> path{ };
		std::filesystem::path cpp_path;
		std::filesystem::path json_path;
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
