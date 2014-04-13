INCLUDE(CheckCXXCompilerFlag)
CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT( fx_examples )

SET(CMAKE_C_COMPILER "gcc")
SET(CMAKE_CXX_COMPILER "g++")

SET(CMAKE_CXX_FLAGS_DEBUG "-Wall -Wextra -Werror -Wno-unused-variable -ggdb")
SET(CMAKE_CXX_FLAGS_RELEASE "-Wall -Wextra -Werror -Wno-unused-variable -DNDEBUG -O3" )

CHECK_CXX_COMPILER_FLAG("-Wno-unused-local-typedefs" support)
IF(support)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-local-typedefs")
ENDIF()

SET(CMAKE_FIND_LIBRARY_PREFIXES "lib")
SET(CMAKE_FIND_LIBRARY_SUFFIXES ".so" ".a")

ADD_SUBDIRECTORY( server )
ADD_SUBDIRECTORY( client )
