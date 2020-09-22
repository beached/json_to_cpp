// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, version 1.0. (see accompanying
// file license or copy at http://www.boost.org/license_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include <optional>

#include <daw/daw_string_view.h>

namespace daw::curl {
	std::optional<std::string> download( daw::string_view url,
	                                     daw::string_view user_agent );

	bool is_url( daw::string_view path );
} // namespace daw::curl
