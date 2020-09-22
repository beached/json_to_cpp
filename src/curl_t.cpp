// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, version 1.0. (see accompanying
// file license or copy at http://www.boost.org/license_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#include <cstdlib>
#include <curl/curl.h>
#include <iostream>
#include <optional>
#include <string>

#include <daw/daw_string_view.h>

#include "curl_t.h"

namespace daw::curl {
	namespace {
		size_t callback( char const *in, size_t const size, size_t const num,
		                 std::string *const out ) {
			assert( out );
			size_t const totalBytes = size * num;
			out->append( in, totalBytes );
			return totalBytes;
		}

		struct curl_t {
			CURL *m_curl;

			curl_t( )
			  : m_curl( curl_easy_init( ) ) {}

			~curl_t( ) {
				curl_easy_cleanup( std::exchange( m_curl, nullptr ) );
			}

			curl_t( curl_t const & ) = delete;
			curl_t( curl_t && ) = delete;
			curl_t &operator=( curl_t const & ) = delete;
			curl_t &operator=( curl_t && ) = delete;

			explicit operator bool( ) const noexcept {
				return static_cast<bool>( m_curl );
			}

			template<typename Opt, typename Param>
			CURLcode setopt( Opt &&opt, Param &&param ) {
				return curl_easy_setopt( m_curl, std::forward<Opt>( opt ),
				                         std::forward<Param>( param ) );
			}

			CURLcode perform( ) {
				return curl_easy_perform( m_curl );
			}

			template<typename Info, typename Arg>
			CURLcode getinfo( Info &&info, Arg &&arg ) {
				return curl_easy_getinfo( m_curl, std::forward<Info>( info ),
				                          std::forward<Arg>( arg ) );
			}
		};

		struct slist_t {
			curl_slist *m_headers = nullptr;

			constexpr slist_t( ) noexcept = default;

			~slist_t( ) {
				curl_slist_free_all( std::exchange( m_headers, nullptr ) );
			}

			slist_t( slist_t const & ) = delete;
			slist_t( slist_t && ) = delete;
			slist_t &operator=( slist_t const & ) = delete;
			slist_t &operator=( slist_t && ) = delete;

			curl_slist *get( ) {
				return m_headers;
			}

			void append( char const *str ) {
				m_headers = curl_slist_append( m_headers, str );
			}
		};
	} // namespace

	std::optional<std::string> download( daw::string_view url,
	                                     daw::string_view user_agent ) {
		auto headers = slist_t( );
		headers.append( "Accept: application/json" );
		headers.append( "Content-Type: application/json" );
		headers.append( "charsets: utf-8" );

		auto curl = curl_t( );
		if( not curl ) {
			return std::nullopt;
		}
		curl.setopt( CURLOPT_HTTPHEADER, headers.get( ) );
		curl.setopt( CURLOPT_HTTPGET, 1 );
		curl.setopt( CURLOPT_HTTPHEADER, headers.get( ) );
		curl.setopt( CURLOPT_USERAGENT, user_agent.data( ) );

		// Set remote URL.
		curl.setopt( CURLOPT_URL, url.data( ) );

		// Don't wait forever, time out after 10 seconds.
		curl.setopt( CURLOPT_TIMEOUT, 15 );

		// Follow HTTP redirects if necessary.
		curl.setopt( CURLOPT_FOLLOWLOCATION, 1L );

		// Response information.
		int httpCode = 0;
		auto httpData = std::string( );

		// Hook up data handling function.
		curl.setopt( CURLOPT_WRITEFUNCTION, callback );

		// Hook up data container ( will be passed as the last parameter to the
		// callback handling function ).  Can be any pointer type, since it will
		// internally be passed as a void pointer.
		curl.setopt( CURLOPT_WRITEDATA, &httpData );

		// Run our HTTP GET command, capture the HTTP response code, and clean up.
		curl.perform( );
		curl.getinfo( CURLINFO_RESPONSE_CODE, &httpCode );

		if( httpCode != 200 ) {
			std::cerr << "Couldn't GET from " << url << '\n';
			return std::nullopt;
		}

		return { std::move( httpData ) };
	}

	bool is_url( daw::string_view path ) {
		static constexpr daw::string_view http_pref = "http://";
		static constexpr daw::string_view https_pref = "https://";
		return path.starts_with( http_pref ) | path.starts_with( https_pref );
	}
} // namespace daw::curl
