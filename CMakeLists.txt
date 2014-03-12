CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT( fxserver )

SET(CMAKE_C_COMPILER "gcc")
SET(CMAKE_CXX_COMPILER "g++")
SET(CMAKE_CXX_FLAGS "-Wall -Werror -O3")

if (CMAKE_COMPILER_IS_GNUCC)
    execute_process(COMMAND ${CMAKE_C_COMPILER} -dumpversion OUTPUT_VARIABLE GCC_VERSION)
    if (GCC_VERSION VERSION_GREATER 4.8 OR GCC_VERSION VERSION_EQUAL 4.8)
        SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-local-typedefs")
    endif()
endif()

SET(CMAKE_FIND_LIBRARY_PREFIXES "lib")
SET(CMAKE_FIND_LIBRARY_SUFFIXES ".so" ".a")

SET(BIN "fxserver")

INCLUDE_DIRECTORIES(/opt/include)
INCLUDE_DIRECTORIES(include)

FIND_LIBRARY(GLOG "glog")
#FIND_PACKAGE(Boost COMPONENTS REQUIRED system thread)
LIST(APPEND SERVER_SRC_LIST src/fxserver.cc 
                            src/fxbuffer.cc 
                            src/fxtimer.cc 
                            src/fx_timer_mgr.cc
                            src/main.cc
                            )

ADD_EXECUTABLE( ${BIN} ${SERVER_SRC_LIST} )
TARGET_LINK_LIBRARIES( ${BIN} 
                        ${GLOG} 
                        #${Boost_LIBRARIES}
                        )
