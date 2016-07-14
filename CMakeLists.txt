cmake_minimum_required(VERSION 2.8)

file(GLOB lib_SRC "src/*.h" "src/*.cpp" "src/internal/*.h" "src/internal/*.cpp")

add_library(Multicast ${lib_SRC})
target_include_directories(Multicast PUBLIC "src")

target_link_libraries (Multicast ${Boost_LIBRARIES})

add_executable(test.libMulticast test/test.cpp)
target_link_libraries (test.libMulticast Multicast)
