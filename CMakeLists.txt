cmake_minimum_required( VERSION 3.10 )

project( json_to_cpp_prj )

include( ExternalProject )

find_package( Boost 1.60.0 COMPONENTS system date_time iostreams program_options filesystem regex unit_test_framework REQUIRED )

find_package( CURL )
find_package( Threads REQUIRED )

enable_testing( )
add_definitions( -DBOOST_TEST_DYN_LINK ) 


set( CMAKE_CXX_STANDARD 17 CACHE STRING "The C++ standard whose features are requested.")

include( "${CMAKE_SOURCE_DIR}/dependent_projects/CMakeListsCompiler.txt" )

if( "${CMAKE_BUILD_TYPE}" STREQUAL "Debug" )
	set( GLEAN_CACHE "${CMAKE_SOURCE_DIR}/.glean/debug" )
	set( GLEAN_CACHE2 "${CMAKE_SOURCE_DIR}/.glean/release" )
elseif( "${CMAKE_BUILD_TYPE}" STREQUAL "Release" )
	set( GLEAN_CACHE "${CMAKE_SOURCE_DIR}/.glean/release" )
	set( GLEAN_CACHE2 "${CMAKE_SOURCE_DIR}/.glean/debug" )
else( )
	set( GLEAN_CACHE "${CMAKE_SOURCE_DIR}/.glean/release" )
	set( GLEAN_CACHE2 "${CMAKE_SOURCE_DIR}/.glean/debug" )
endif( )

message( "Building for: ${CMAKE_BUILD_TYPE}" )
message( "Checking for glean cache: '${GLEAN_CACHE}'" )
if( EXISTS "${GLEAN_CACHE}" )
	message( "using glean dependencies at ${GLEAN_CACHE}" )
	add_custom_target( dependency_stub )
	include_directories( SYSTEM "${GLEAN_CACHE}/include" )
	link_directories( "${GLEAN_CACHE}/lib" )
elseif( EXISTS "${GLEAN_CACHE2}" )
	message( "looks like glean is in use but missing ${GLEAN_CACHE}" )
elseif( EXISTS "${CMAKE_SOURCE_DIR}/glean.cmake" )
	message( "glean.cmake detected" )
	include( "${CMAKE_SOURCE_DIR}/glean.cmake" )
	add_library( dependency_stub STATIC "${CMAKE_SOURCE_DIR}/dependent_projects/stub.cpp" )
	add_dependencies( dependency_stub ${DEP_PROJECT_DEPS} )
	include_directories( SYSTEM "${CMAKE_BINARY_DIR}/install/include" )
	link_directories( "${CMAKE_BINARY_DIR}/install/lib" )
endif( )

set( HEADER_FOLDER "include" )
set( SOURCE_FOLDER "src" )
set( TEST_FOLDER "tests" )

include_directories( SYSTEM "${CMAKE_BINARY_DIR}/install/include" )
include_directories( ${HEADER_FOLDER} )
include_directories( SYSTEM ${OPENSSL_INCLUDE_DIR} )
include_directories( SYSTEM ${CURL_INCLUDE_DIRS} )
include_directories( SYSTEM ${Boost_INCLUDE_DIRS} )

link_directories( "${CMAKE_BINARY_DIR}/install/lib" )
link_directories( ${Boost_LIBRARY_DIRS} )

set( HEADER_FILES
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

set( SOURCE_FILES
	${SOURCE_FOLDER}/curl_t.cpp
	${SOURCE_FOLDER}/json_to_cpp.cpp
	${SOURCE_FOLDER}/ti_array.cpp
	${SOURCE_FOLDER}/ti_kv.cpp
	${SOURCE_FOLDER}/ti_object.cpp
)

add_library( json_to_cpp_lib ${HEADER_FILES} ${SOURCE_FILES} )
add_dependencies( json_to_cpp_lib dependency_stub )
target_link_libraries( json_to_cpp_lib utf_range utf_string ${Boost_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT} )

add_executable( json_to_cpp ${HEADER_FILES} ${SOURCE_FOLDER}/main.cpp )
add_dependencies( json_to_cpp dependency_stub )
target_link_libraries( json_to_cpp json_to_cpp_lib parse_json utf_range ${Boost_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT} ${CURL_LIBRARIES} )

#install( TARGETS json_to_cpp DESTINATION lib )
install( TARGETS json_to_cpp DESTINATION bin )
#install( DIRECTORY ${HEADER_FOLDER}/ DESTINATION include/daw/json_to_cpp )

