// The MIT License (MIT)
//
// Copyright (c) 2016-2017 Darrell Wright
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files( the "Software" ), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#pragma once

#include <map>
#include <string>

#include <daw/json/value_t.h>

#include "daw_json_parser_v2_state.h"

namespace daw {
	namespace json_to_cpp {
		namespace types {
			struct type_info_t;

			struct ti_value {
				type_info_t *value;

				std::string name( ) const noexcept;
				std::map<std::string, ti_value> const &children( ) const;
				std::map<std::string, ti_value> &children( );
				bool &is_optional( ) noexcept;
				bool const &is_optional( ) const noexcept;
				daw::json::impl::value_t::value_types type( ) const;

				ti_value( );
				~ti_value( );
				ti_value( ti_value const &other );
				ti_value( ti_value &&other );

				ti_value &operator=( ti_value const &rhs );
				ti_value &operator=( ti_value &&rhs );

				template<typename Derived>
				ti_value( Derived other ) : value{new Derived( std::move( other ) )} {}

				template<typename Derived>
				ti_value &operator=( Derived rhs ) {
					value = new Derived( std::move( rhs ) );
					return *this;
				}
			}; // ti_value

			template<typename Derived, typename... Args>
			ti_value create_ti_value( Args &&... args ) {
				ti_value result;
				auto tmp = new Derived( std::forward<Args>( args )... );
				result.value = std::exchange( tmp, nullptr );
				return result;
			}

			struct type_info_t {
				bool is_optional;
				std::map<std::string, ti_value> children;

				type_info_t( );
				type_info_t( type_info_t const & ) = default;
				type_info_t( type_info_t && ) = default;
				type_info_t &operator=( type_info_t const & ) = default;
				type_info_t &operator=( type_info_t && ) = default;
				virtual ~type_info_t( );

				virtual daw::json::impl::value_t::value_types type( ) const = 0;
				virtual std::string name( ) const = 0;

				virtual type_info_t *clone( ) const = 0;
			}; // type_info_t

			struct ti_null final : public type_info_t {
				daw::json::impl::value_t::value_types type( ) const override;
				std::string name( ) const override;
				ti_null( );
				type_info_t *clone( ) const override;
			}; // struct ti_null

			struct ti_integral final : public type_info_t {
				daw::json::impl::value_t::value_types type( ) const override;
				std::string name( ) const override;
				type_info_t *clone( ) const override;
			}; // struct ti_integral

			struct ti_real final : public type_info_t {
				daw::json::impl::value_t::value_types type( ) const override;
				std::string name( ) const override;
				type_info_t *clone( ) const override;
			}; // struct ti_real

			struct ti_boolean final : public type_info_t {
				daw::json::impl::value_t::value_types type( ) const override;
				std::string name( ) const override;
				type_info_t *clone( ) const override;
			}; // struct ti_boolean

			struct ti_string final : public type_info_t {
				daw::json::impl::value_t::value_types type( ) const override;
				std::string name( ) const override;
				type_info_t *clone( ) const override;
			}; // struct ti_string

			struct ti_object final : public type_info_t {
				std::string object_name;
				type_info_t *parent;
				daw::json::state::state_t *parent_state;
				std::string member_name_stack;

				daw::json::impl::value_t::value_types type( ) const override;
				std::string name( ) const override;
				ti_object( std::string obj_name, type_info_t *Parent );
				type_info_t *clone( ) const override;
			}; // struct ti_object

			struct ti_array final : public type_info_t {
				type_info_t *parent;
				daw::json::state::state_t *parent_state;

				ti_array( type_info_t *Parent );
				daw::json::impl::value_t::value_types type( ) const override;
				std::string name( ) const override;
				type_info_t *clone( ) const override;
			}; // ti_array

		} // namespace types
	}     // namespace json_to_cpp
} // namespace daw

