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

namespace daw::json_to_cpp::types {
	struct ti_boolean {
		bool is_optional = false;

		constexpr ti_boolean( ) noexcept = default;

		static constexpr bool is_null = false;
		static constexpr size_t type = impl::ti_boolean_pos;

		static inline std::string name( ) noexcept {
			return "bool";
		}

		static inline std::string array_member_info( ) noexcept {
			return "bool";
		}

		inline static std::string
		json_name( daw::string_view member_name, bool use_cpp20,
		           daw::string_view parent_name ) noexcept {
			return "json_bool<" +
			       impl::format_member_name( member_name, use_cpp20, parent_name ) +
			       ">";
		}
	};
} // namespace daw::json_to_cpp::types
