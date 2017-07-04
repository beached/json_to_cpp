cmake_minimum_required( VERSION 2.8.12 )

project( json_to_cpp_prj )

include( ExternalProject )

find_package( Boost 1.58.0 COMPONENTS system date_time iostreams program_options filesystem regex unit_test_framework REQUIRED )

find_package( CURL )

enable_testing( )
add_definitions( -DBOOST_TEST_DYN_LINK ) 

find_package( Threads REQUIRED )

if( ${CMAKE_CXX_COMPILER_ID} STREQUAL 'MSVC' )
	add_compile_options( -D_WIN32_WINNT=0x0601 ) 
else( )
	if( ${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang" )
		add_compile_options(-std=c++14 -Weverything -pthread -Wno-c++98-compat -g -Wno-covered-switch-default -Wno-padded -Wno-exit-time-destructors -Wno-c++98-compat-pedantic -Wno-unused-parameter -Wno-missing-noreturn -Wno-missing-prototypes -Wno-disabled-macro-expansion -Wno-c99-compat)
	elseif( ${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU" )
		add_compile_options(-std=c++14 -ggdb -Wall -pthread -Wno-deprecated-declarations -march=native )
	endif( )
endif( )

externalproject_add(
    header_libraries_prj
    GIT_REPOSITORY "https://github.com/beached/header_libraries.git"
    SOURCE_DIR "${CMAKE_BINARY_DIR}/dependencies/header_libraries"
    GIT_TAG "master"
    INSTALL_DIR "${CMAKE_BINARY_DIR}/install"
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}/install
)

externalproject_add(
    parse_json_prj
    GIT_REPOSITORY "https://github.com/beached/parse_json.git"
    SOURCE_DIR "${CMAKE_BINARY_DIR}/dependencies/parse_json"
    GIT_TAG "master"
    INSTALL_DIR "${CMAKE_BINARY_DIR}/install"
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}/install
)

externalproject_add(
    char_range_prj
    GIT_REPOSITORY "https://github.com/beached/char_range.git"
    SOURCE_DIR "${CMAKE_BINARY_DIR}/dependencies/char_range"
    GIT_TAG "master"
    INSTALL_DIR "${CMAKE_BINARY_DIR}/install"
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}/install
)

externalproject_add(
    date_prj
    GIT_REPOSITORY "https://github.com/beached/date.git"
    SOURCE_DIR "${CMAKE_BINARY_DIR}/dependencies/date"
    GIT_TAG "master"
    INSTALL_DIR "${CMAKE_BINARY_DIR}/install"
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}/install
)

externalproject_add(
    temp_file_prj
    GIT_REPOSITORY "https://github.com/beached/libtemp_file.git"
    SOURCE_DIR "${CMAKE_BINARY_DIR}/dependencies/temp_file"
    GIT_TAG "master"
    INSTALL_DIR "${CMAKE_BINARY_DIR}/install"
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}/install
)

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
	${HEADER_FOLDER}/json_to_cpp.h
)

set( SOURCE_FILES
	${SOURCE_FOLDER}/json_to_cpp.cpp
)

add_library( json_to_cpp ${HEADER_FILES} ${SOURCE_FILES} )
add_dependencies( json_to_cpp header_libraries_prj parse_json_prj char_range_prj date_prj )
target_link_libraries( json_to_cpp parse_json tz char_range ${Boost_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT} )

add_executable( json_to_cpp_bin ${HEADER_FILES} ${SOURCE_FOLDER}/main.cpp )
add_dependencies( json_to_cpp_bin temp_file_prj )
target_link_libraries( json_to_cpp_bin json_to_cpp parse_json char_range temp_file ${Boost_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT} ${CURL_LIBRARIES} )

install( TARGETS json_to_cpp DESTINATION lib )
install( TARGETS json_to_cpp_bin DESTINATION bin )
install( DIRECTORY ${HEADER_FOLDER}/ DESTINATION include/daw/json_to_cpp )
