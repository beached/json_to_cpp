// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, version 1.0. (see accompanying
// file license or copy at http://www.boost.org/license_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include <variant>

#include "ti_array.h"
#include "ti_base.h"
#include "ti_boolean.h"
#include "ti_integral.h"
#include "ti_kv.h"
#include "ti_null.h"
#include "ti_object.h"
#include "ti_real.h"
#include "ti_string.h"

namespace daw::json_to_cpp::types {
	using ti_types_t = std::variant<ti_null, ti_array, ti_boolean, ti_integral,
	                                ti_object, ti_real, ti_string, ti_kv>;

	static_assert(
	  impl::validate_pos_value_v<ti_types_t, ti_null, impl::ti_null_pos> );
	static_assert(
	  impl::validate_pos_value_v<ti_types_t, ti_array, impl::ti_array_pos> );
	static_assert(
	  impl::validate_pos_value_v<ti_types_t, ti_boolean, impl::ti_boolean_pos> );
	static_assert( impl::validate_pos_value_v<ti_types_t, ti_integral,
	                                          impl::ti_integral_pos> );
	static_assert(
	  impl::validate_pos_value_v<ti_types_t, ti_object, impl::ti_object_pos> );
	static_assert(
	  impl::validate_pos_value_v<ti_types_t, ti_object, impl::ti_object_pos> );
	static_assert(
	  impl::validate_pos_value_v<ti_types_t, ti_real, impl::ti_real_pos> );
	static_assert(
	  impl::validate_pos_value_v<ti_types_t, ti_string, impl::ti_string_pos> );
	static_assert(
	  impl::validate_pos_value_v<ti_types_t, ti_kv, impl::ti_kv_pos> );

	static_assert( impl::are_same(
	  impl::var_to_pack_t<ti_types_t>{ },
	  impl::var_to_pack_t<typename ti_array::child_items_t>{ } ) );

	static_assert(
	  impl::are_same( impl::var_to_pack_t<ti_types_t>{ },
	                  impl::var_to_pack_t<typename ti_kv::child_items_t>{ } ) );

	static_assert( impl::are_same(
	  impl::var_to_pack_t<ti_types_t>{ },
	  impl::var_to_pack_t<typename ti_object::child_items_t>{ } ) );

} // namespace daw::json_to_cpp::types
