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

	static_assert(
	  impl::are_same( impl::var_to_pack_t<ti_types_t>{},
	                  impl::var_to_pack_t<typename ti_array::child_items_t>{} ) );

	static_assert(
	  impl::are_same( impl::var_to_pack_t<ti_types_t>{},
	                  impl::var_to_pack_t<typename ti_kv::child_items_t>{} ) );

	static_assert( impl::are_same(
	  impl::var_to_pack_t<ti_types_t>{},
	  impl::var_to_pack_t<typename ti_object::child_items_t>{} ) );

} // namespace daw::json_to_cpp::types
