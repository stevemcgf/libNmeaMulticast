cmake_minimum_required(VERSION 2.8)

# Get the latest abbreviated commit hash of the working branch
execute_process(
	COMMAND git log -1 --format=%h
	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
	OUTPUT_VARIABLE GIT_COMMIT_HASH
	OUTPUT_STRIP_TRAILING_WHITESPACE
)

# Get the latest tag version
execute_process(
	COMMAND git describe --tags
	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
	OUTPUT_VARIABLE GIT_TAG_VERSION
	OUTPUT_STRIP_TRAILING_WHITESPACE
)

if (NOT "${GIT_TAG_VERSION}" STREQUAL "")
	string(REGEX REPLACE "^v([0-9]+)\\..*" "\\1" VERSION_MAJOR "${GIT_TAG_VERSION}")
	string(REGEX REPLACE "^v[0-9]+\\.([0-9]+).*" "\\1" VERSION_MINOR "${GIT_TAG_VERSION}")
	string(REGEX REPLACE "^v[0-9]+\\.[0-9]+\\.([0-9]+).*" "\\1" VERSION_PATCH "${GIT_TAG_VERSION}")
	set(VERSION_STRING "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}")
	set(VERSION_FULL "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}-${GIT_COMMIT_HASH}")
endif (NOT "${GIT_TAG_VERSION}" STREQUAL "")

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Version.h.in ${CMAKE_CURRENT_BINARY_DIR}/Version.h @ONLY)

file(GLOB lib_SRC "include/*.h" "src/*.cpp")

add_compile_options(-std=c++11)
add_compile_options(-DBOOST_LOG_DYN_LINK)

find_package(Boost 1.55 REQUIRED COMPONENTS log system thread)

include_directories(${Boost_INCLUDE_DIRS})

add_library(NmeaMulticast ${lib_SRC})
target_include_directories(NmeaMulticast PUBLIC "include")

target_link_libraries (NmeaMulticast ${Boost_LIBRARIES})

add_executable(test.libNmeaMulticast test/test.cpp)
target_link_libraries (test.libNmeaMulticast NmeaMulticast)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DNM_DEBUG")

# add a target to generate API documentation with Doxygen

find_package(Doxygen)

if(DOXYGEN_FOUND)

	configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)
	add_custom_target(docs.NmeaMulticast
		${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
		WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
		COMMENT "Generating API documentation with Doxygen" VERBATIM
		)

endif(DOXYGEN_FOUND)
