project (TREVI)

include_directories(${TREVI_SOURCE_DIR}/include)
INCLUDE_DIRECTORIES(${TREVI_SOURCE_DIR}/extdeps/concurrentqueue)

SET( CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -pthread -msse4.1" )

file (GLOB_RECURSE TREVI_SRC ${TREVI_SOURCE_DIR}/src/*.c*)
file (GLOB_RECURSE TREVI_HEADERS ${TREVI_SOURCE_DIR}/include/*.h)

add_executable(trevi ${TREVI_SRC} ${TREVI_HEADERS} )
