// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, version 1.0. (see accompanying
// file license or copy at http://www.boost.org/license_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include <cstddef>
#include <string>

#include <daw/daw_string_view.h>
#include <daw/json/daw_json_value_t.h>

#include "ti_base.h"

namespace daw::json_to_cpp::types {
	struct ti_integral {
		bool is_optional = false;

		constexpr ti_integral( ) noexcept = default;

		static constexpr bool is_null = false;
		static constexpr size_t type = impl::ti_integral_pos;

		static inline std::string name( ) noexcept {
			return "int64_t";
		}

		static inline std::string array_member_info( ) noexcept {
			return "int64_t";
		}

		inline static std::string
		json_name( daw::string_view member_name, bool use_cpp20,
		           daw::string_view parent_name ) noexcept {
			return "json_number<" +
			       impl::format_member_name( member_name, use_cpp20, parent_name ) +
			       ", int64_t>";
		}
	};
} // namespace daw::json_to_cpp::types
