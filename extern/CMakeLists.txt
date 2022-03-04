# Copyright (c) Darrell Wright
#
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
#
# Official repository: https://github.com/beached/json_to_cpp
#

# This prevents tests running on dependencies
option(DAW_ENABLE_TESTING "Build unit tests" OFF)
if (DAW_ENABLE_TESTING)
    set(DAW_ENABLE_TESTING OFF)
endif ()

include(FetchContent)

FetchContent_Declare(
        daw_parse_json
        GIT_REPOSITORY https://github.com/beached/parse_json.git
        GIT_TAG v1.1.0
)

FetchContent_Declare(
        daw_libtemp_file
        GIT_REPOSITORY https://github.com/beached/libtemp_file.git
)

FetchContent_MakeAvailable(daw_parse_json daw_libtemp_file)

