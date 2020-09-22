// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, version 1.0. (see accompanying
// file license or copy at http://www.boost.org/license_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include "json_to_cpp_config.h"

#include <daw/daw_string_view.h>

namespace daw::json_to_cpp {
	void generate_cpp( daw::string_view json_string, config_t &config );
} // namespace daw::json_to_cpp
