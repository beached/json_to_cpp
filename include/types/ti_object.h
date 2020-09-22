// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, version 1.0. (see accompanying
// file license or copy at http://www.boost.org/license_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include <cstddef>
#include <memory>
#include <string>

#include <daw/daw_ordered_map.h>
#include <daw/json/daw_json_value_t.h>

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
	struct ti_object {
		using child_items_t =
		  std::variant<ti_null, ti_array, ti_boolean, ti_integral, ti_object,
		               ti_real, ti_string, ti_kv>;
		using child_t = daw::ordered_map<std::string, child_items_t>;

		std::unique_ptr<child_t> children;

		bool is_optional = false;
		std::string object_name;

		static constexpr bool is_null = false;
		static constexpr size_t type = impl::ti_object_pos;

		explicit ti_object( std::string obj_name );
		ti_object( ti_object && ) noexcept = default;
		ti_object &operator=( ti_object && ) noexcept = default;
		~ti_object( ) = default;
		ti_object( ti_object const &other );
		ti_object &operator=( ti_object const &rhs );

		std::string name( ) const;
		std::string json_name( daw::string_view member_name, bool use_cpp20,
		                       daw::string_view parent_name ) const;
		std::string array_member_info( ) const;
	};
} // namespace daw::json_to_cpp::types
