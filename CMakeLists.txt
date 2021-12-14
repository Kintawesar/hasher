cmake_minimum_required(VERSION 3.16)

project(hasher)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

add_compile_options(-Wall -Wextra -pedantic -fPIC  -fstack-clash-protection -fexceptions -fstack-protector-strong)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -g -D_GLIBCXX_ASSERTIONS")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -g -D_FORTIFY_SOURCE=2")

if(NOT CMAKE_BUILD_TYPE)
    message(STATUS "Setting build type to 'RelWithDebInfo' as none was specified")
    set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_FLAGS_TSAN
        "-fsanitize=thread -g -O1"
        CACHE STRING "Flags used by the C++ compiler during ThreadSanitizer builds."
        FORCE)

add_executable(hasher main.cpp hashlog.cpp map_file.cpp)
target_compile_features(hasher PRIVATE cxx_std_17 )


target_link_libraries(hasher PRIVATE ${CONAN_LIBS})
