# Copyright (c) Darrell Wright
#
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
#
# Official repository: https://github.com/beached/json_to_cpp
#

# This prevents tests running on dependencies
option( DAW_ENABLE_TESTING "Build unit tests" OFF )
if( DAW_ENABLE_TESTING )
	set( DAW_ENABLE_TESTING OFF )
endif( )

include( FetchContent )
set( FETCHCONTENT_UPDATES_DISCONNECTED ON )

FetchContent_Declare(
	daw_header_libraries
	GIT_REPOSITORY https://github.com/beached/header_libraries.git
)

FetchContent_Declare(
	daw_utf_range
	GIT_REPOSITORY https://github.com/beached/utf_range.git
)

FetchContent_Declare(
	daw_parse_json
	GIT_REPOSITORY https://github.com/beached/parse_json.git
)

FetchContent_Declare(
	daw_libtemp_file
	GIT_REPOSITORY https://github.com/beached/libtemp_file.git
)

FetchContent_Declare(
	fmtlib
	GIT_REPOSITORY https://github.com/fmtlib/fmt.git
)	

option( ENABLE_DATE_TESTING "" OFF )
option( USE_SYSTEM_TZ_DB "" ON )
FetchContent_Declare(
	date
	GIT_REPOSITORY https://github.com/howardhinnant/date.git
)

FetchContent_MakeAvailable(daw_header_libraries daw_utf_range daw_parse_json daw_libtemp_file date fmtlib )

