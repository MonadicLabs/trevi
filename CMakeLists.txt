cmake_minimum_required(VERSION 3.0)

project (TREVI)

option(USE_LOGGING "Build with logging capability" OFF)
option(USE_PROFILING "Build with profiling capability" OFF)
option(BUILD_EXAMPLE_APPS "Build Example Applications" ON)
option(ARM_CROSS_COMPILE "Is the project cross compiling ?" OFF)
option(BUILD_STATIC_LIB "Build novalink library as a static library" ON)

## CHECK FOR COMPILER CAPABILITIES
include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
if(COMPILER_SUPPORTS_CXX11)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
elseif(COMPILER_SUPPORTS_CXX0X)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
else()
  message(FATAL_ERROR "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
endif()
##

add_subdirectory(lib)

if( BUILD_EXAMPLE_APPS )
    add_subdirectory( examples )
endif( BUILD_EXAMPLE_APPS )
