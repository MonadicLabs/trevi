cmake_minimum_required(VERSION 2.8)
project (LIBTREVI)

# Set custom cmake scripts directory
set( CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${LIBNOVALINK_SOURCE_DIR}/cmake")
SET( CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -pthread -fpermissive -flax-vector-conversions -Wcast-align" )
SET( CMAKE_EXE_LINKER_FLAGS "-pthread")

# C99
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")

# C++11
include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
if(COMPILER_SUPPORTS_CXX11)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
elseif(COMPILER_SUPPORTS_CXX0X)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
else()
    message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
endif()

FIND_PACKAGE(OpenMP)
  IF(OPENMP_FOUND)
  SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
ENDIF()

# Add Dynamic Library Loader dependency, pthread, uuid (if UNIX-based)
set( TREVI_DEPS "${TREVI_DEPS};${OpenMP_CXX_LIBRARIES}" )

# Set include directories
include_directories( ${LIBTREVI_SOURCE_DIR}/include )

# REMOTERY
if( USE_PROFILING )
INCLUDE_DIRECTORIES( ${TREVI_SOURCE_DIR}/remotery/lib )
file (GLOB_RECURSE REMOTERY_SRC ${TREVI_SOURCE_DIR}/remotery/lib/*.c)
file (GLOB_RECURSE REMOTERY_HEADERS ${TREVI_SOURCE_DIR}/remotery/lib/*.h*)
set( TREVI_DEPS "${TREVI_DEPS};pthread" )
add_definitions( -DUSE_PROFILING )
endif()

# DOLLAR
if( USE_PROFILING )
INCLUDE_DIRECTORIES( ${TREVI_SOURCE_DIR}/dollar )
file (GLOB_RECURSE DOLLAR_SRC ${TREVI_SOURCE_DIR}/dollar/*.c*)
file (GLOB_RECURSE DOLLAR_HEADERS ${TREVI_SOURCE_DIR}/dollar/*.h*)
add_definitions( -DUSE_PROFILING )
endif()

# Loguru
if( USE_LOGGING )
    INCLUDE_DIRECTORIES( ${TREVI_SOURCE_DIR}/loguru )
    file (GLOB_RECURSE LOGURU_HEADERS ${TREVI_SOURCE_DIR}/loguru/*.h*)
    add_definitions( -DUSE_LOGGING )
    set( TREVI_DEPS "${TREVI_DEPS}" )
endif()

# SIMDE
INCLUDE_DIRECTORIES( ${TREVI_SOURCE_DIR}/simde/simde )
file (GLOB_RECURSE SIMDE_HEADERS ${TREVI_SOURCE_DIR}/simde/simde/*.h*)

# Global source fetching
file ( GLOB_RECURSE LIBTREVI_CPP ./src/*.c* )
file ( GLOB_RECURSE LIBTREVI_HEADERS ./include/*.h )

# SET SOURCES
set( LIBTREVI_ALL_CPP ${LIBTREVI_CPP} ${LIBTREVI_HEADERS} ${DOLLAR_SRC} ${REMOTERY_SRC} )

if( BUILD_STATIC_LIB )
    add_library( trevi STATIC ${LIBTREVI_ALL_CPP} )
else()
    add_library( trevi SHARED ${LIBTREVI_ALL_CPP} )
endif()

# Link libraries
target_link_libraries( trevi ${TREVI_DEPS} )
