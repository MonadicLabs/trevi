
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

link_directories( ${LIBTREVI_BINARY_DIR} )
include_directories( ${LIBTREVI_SOURCE_DIR}/include )

set( DEPS "${DEPS};trevi" )

SET( CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -fpermissive -pthread -flax-vector-conversions" )

add_executable( trevi_tx main.cc Timer.cpp udptransmitter.cc udpreceiver.cc udputils.cc )
target_link_libraries( trevi_tx ${DEPS} )

INSTALL(TARGETS trevi_tx DESTINATION "bin" )
