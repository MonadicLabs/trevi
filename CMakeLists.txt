project (TREVI)

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

include_directories(${TREVI_SOURCE_DIR}/include)
INCLUDE_DIRECTORIES(${TREVI_SOURCE_DIR}/extdeps/concurrentqueue)

SET( CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -pthread -msse4.1" )

file (GLOB_RECURSE TREVI_SRC ${TREVI_SOURCE_DIR}/src/*.c*)
file (GLOB_RECURSE TREVI_HEADERS ${TREVI_SOURCE_DIR}/include/*.h)

add_executable(trevi ${TREVI_SRC} ${TREVI_HEADERS} )
