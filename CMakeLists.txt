cmake_minimum_required(VERSION 3.9)

set(CMAKE_VERBOSE_MAKEFILE on)

project(x265ConverterWrapper VERSION 1.0 LANGUAGES CXX)

# Include Conan info
include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup(TARGETS)

add_executable(x265ConverterWrapper 
    src/main.cpp)

if(NOT CONAN_BOOST_ROOT)
    find_package(boost COMPONENTS process REQUIRED)
endif()
if(NOT CONAN_NLOHMANN_JSON_ROOT)
    find_package(nlohmann_json REQUIRED)
endif()
if(NOT CONAN_LYRA_ROOT)
    find_package(lyra REQUIRED)
endif()

target_link_libraries(${PROJECT_NAME} PRIVATE stdc++fs)

target_include_directories(${PROJECT_NAME} PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/include/
    ${CONAN_BOOST_ROOT}/include
    ${CONAN_LYRA_ROOT}/include)

target_compile_options(${PROJECT_NAME} PRIVATE
     $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>:
          -Wall -Wextra>
     $<$<CXX_COMPILER_ID:MSVC>:
          /W4>)

# require C++17
target_compile_features(${PROJECT_NAME} INTERFACE cxx_std_17)

