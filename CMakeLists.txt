cmake_minimum_required( VERSION 2.8.12 )

project( json_to_cpp_prj )

include( ExternalProject )

find_package( Boost 1.58.0 COMPONENTS system date_time iostreams program_options filesystem regex unit_test_framework REQUIRED )

find_package( CURL )
find_package( Threads REQUIRED )

enable_testing( )
add_definitions( -DBOOST_TEST_DYN_LINK ) 

include( "${CMAKE_SOURCE_DIR}/dependent_projects/CMakeListsCompiler.txt" )

include_directories( "./include" )
set( DEP_ROOT_DIR_BASE "${CMAKE_SOURCE_DIR}/dependent_projects/build" )
if( "${CMAKE_BUILD_TYPE}" STREQUAL "Debug" )
	set( DEP_ROOT_DIR "${DEP_ROOT_DIR_BASE}/debug" )
elseif( "${CMAKE_BUILD_TYPE}" STREQUAL "Release" )
	set( DEP_ROOT_DIR "${DEP_ROOT_DIR_BASE}/release" )
elseif( "${CMAKE_BUILD_TYPE}" STREQUAL "" )
	set( DEP_ROOT_DIR "${DEP_ROOT_DIR_BASE}/release" )
endif( )

message( "Building for: ${CMAKE_BUILD_TYPE}" )
if( EXISTS "${DEP_ROOT_DIR}" )
	message( "${DEP_ROOT_DIR}" )
	message( "using dependency folder" )
	add_custom_target( dependency_stub )
	include_directories( SYSTEM "${DEP_ROOT_DIR}/install/include" )
	link_directories( "${DEP_ROOT_DIR}/install/lib" )
else( )
	message( "downloading and building deps" )
	include( "${CMAKE_SOURCE_DIR}/dependent_projects/dependencies.txt" )
	add_library( dependency_stub STATIC "${CMAKE_SOURCE_DIR}/dependent_projects/stub.cpp" )
	add_dependencies( dependency_stub ${DEP_PROJECT_DEPS} )
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
	${HEADER_FOLDER}/ti_value.h
	${HEADER_FOLDER}/ti_array.h
	${HEADER_FOLDER}/ti_boolean.h
	${HEADER_FOLDER}/ti_integral.h
	${HEADER_FOLDER}/ti_null.h
	${HEADER_FOLDER}/ti_object.h
	${HEADER_FOLDER}/ti_real.h
	${HEADER_FOLDER}/ti_string.h
	${HEADER_FOLDER}/ti_types.h
)

set( SOURCE_FILES
	${SOURCE_FOLDER}/curl_t.cpp
	${SOURCE_FOLDER}/json_to_cpp.cpp
	${SOURCE_FOLDER}/ti_array.cpp
	${SOURCE_FOLDER}/ti_object.cpp
)

add_library( json_to_cpp ${HEADER_FILES} ${SOURCE_FILES} )
add_dependencies( json_to_cpp dependency_stub )
target_link_libraries( json_to_cpp tz utf_range utf_string ${Boost_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT} )

add_executable( json_to_cpp_bin ${HEADER_FILES} ${SOURCE_FOLDER}/main.cpp )
add_dependencies( json_to_cpp_bin dependency_stub )
target_link_libraries( json_to_cpp_bin json_to_cpp parse_json utf_range temp_file ${Boost_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT} ${CURL_LIBRARIES} )

install( TARGETS json_to_cpp DESTINATION lib )
install( TARGETS json_to_cpp_bin DESTINATION bin )
install( DIRECTORY ${HEADER_FOLDER}/ DESTINATION include/daw/json_to_cpp )

