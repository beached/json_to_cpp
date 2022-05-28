# Copyright (c) Darrell Wright
#
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
#
# Official repository: https://github.com/beached/json_to_cpp
#
cmake_minimum_required(VERSION 3.13)

project("json_to_cpp"
    VERSION "2.1.1"
    DESCRIPTION "Try to use the JSON structures to make C++ structures and bindings"
    HOMEPAGE_URL "https://github.com/beached/json_to_cpp"
    LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17 CACHE STRING "The C++ standard whose features are requested.")
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(Boost_NO_WARN_NEW_VERSIONS ON)

find_package(Boost 1.60.0 COMPONENTS program_options REQUIRED)
find_package(CURL REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(date REQUIRED)
find_package(fmt REQUIRED)
add_subdirectory(extern)

set(HEADER_FOLDER include)
set(SOURCE_FOLDER src)

set(HEADER_FILES
    ${HEADER_FOLDER}/curl_t.h
    ${HEADER_FOLDER}/json_to_cpp.h
    ${HEADER_FOLDER}/json_to_cpp_config.h
    ${HEADER_FOLDER}/ti_value.h
    ${HEADER_FOLDER}/types/ti_array.h
    ${HEADER_FOLDER}/types/ti_base.h
    ${HEADER_FOLDER}/types/ti_boolean.h
    ${HEADER_FOLDER}/types/ti_integral.h
    ${HEADER_FOLDER}/types/ti_kv.h
    ${HEADER_FOLDER}/types/ti_null.h
    ${HEADER_FOLDER}/types/ti_object.h
    ${HEADER_FOLDER}/types/ti_real.h
    ${HEADER_FOLDER}/types/ti_string.h
    ${HEADER_FOLDER}/types/ti_types.h
    )

set(SOURCE_FILES
    ${SOURCE_FOLDER}/curl_t.cpp
    ${SOURCE_FOLDER}/json_to_cpp.cpp
    ${SOURCE_FOLDER}/ti_array.cpp
    ${SOURCE_FOLDER}/ti_kv.cpp
    ${SOURCE_FOLDER}/ti_object.cpp
    ${SOURCE_FOLDER}/main.cpp
    )

add_executable(${PROJECT_NAME} ${HEADER_FILES} ${SOURCE_FILES})
target_link_libraries(${PROJECT_NAME} PRIVATE
    Boost::program_options
    CURL::libcurl
    date::date
    daw::daw-header-libraries
    daw::daw-json-link
    daw::daw-utf-range
    daw::libtemp_file
    fmt::fmt
    OpenSSL::Crypto
    OpenSSL::SSL
    )
target_include_directories(${PROJECT_NAME} PRIVATE ${HEADER_FOLDER})

install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)

