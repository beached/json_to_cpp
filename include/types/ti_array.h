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
#include <variant>

#include <daw/daw_ordered_map.h>
#include <daw/daw_string_view.h>

#include "ti_base.h"
#include "ti_boolean.h"
#include "ti_integral.h"
#include "ti_null.h"
#include "ti_real.h"
#include "ti_string.h"

namespace daw::json_to_cpp::types {
	struct ti_array;
	struct ti_object;
	struct ti_kv;
	struct ti_array {
		using child_items_t =
		  std::variant<ti_null, ti_array, ti_boolean, ti_integral, ti_object,
		               ti_real, ti_string, ti_kv>;

		using child_t = daw::ordered_map<std::string, child_items_t>;

		std::unique_ptr<child_t> children;

		bool is_optional = false;
		static constexpr bool is_null = false;
		static constexpr size_t type = impl::ti_array_pos;

		ti_array( );
		ti_array( ti_array && ) noexcept = default;
		ti_array &operator=( ti_array && ) noexcept = default;
		~ti_array( ) = default;
		ti_array( ti_array const &other );
		ti_array &operator=( ti_array const &rhs );

		std::string name( ) const;
		std::string json_name( daw::string_view member_name, bool use_cpp20,
		                       daw::string_view parent_name ) const;
		std::string array_member_info( ) const;
	};
} // namespace daw::json_to_cpp::types
