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
#include <daw/json/daw_json_link.h>

#include "ti_base.h"

namespace daw::json_to_cpp::types {
	class ti_string {
		bool m_use_string_view;

	public:
		bool is_optional = false;
		static constexpr bool is_null = false;
		static constexpr size_t type = impl::ti_string_pos;

		constexpr explicit ti_string( bool use_string_view ) noexcept
		  : m_use_string_view( use_string_view ) {}

		inline std::string name( ) const noexcept {
			if( m_use_string_view ) {
				return "std::string_view";
			}
			return "std::string";
		}

		inline std::string array_member_info( ) const noexcept {
			if( m_use_string_view ) {
				return "json_string<no_name, std::string_view>";
			}
			return "std::string";
		}

		inline std::string
		json_name( daw::string_view member_name, bool use_cpp20,
		           daw::string_view parent_name ) const noexcept {
			if( m_use_string_view ) {
				return "json_string<" +
				       impl::format_member_name( member_name, use_cpp20, parent_name ) +
				       ", std::string_view>";
			}
			return "json_string<" +
			       impl::format_member_name( member_name, use_cpp20, parent_name ) +
			       ">";
		}
	};
} // namespace daw::json_to_cpp::types
