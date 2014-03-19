INCLUDE(CheckCXXCompilerFlag)
CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
SET(CMAKE_C_COMPILER "gcc")
SET(CMAKE_CXX_COMPILER "g++")
PROJECT( fxserver )

SET(CMAKE_CXX_FLAGS "-Wall -Werror -O2")

CHECK_CXX_COMPILER_FLAG("-Wno-unused-local-typedefs" support)
IF(support)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-local-typedefs")
ENDIF()

SET(CMAKE_FIND_LIBRARY_PREFIXES "lib")
SET(CMAKE_FIND_LIBRARY_SUFFIXES ".so" ".a")

SET(BIN "fxserver")

INCLUDE_DIRECTORIES(/opt/include)
INCLUDE_DIRECTORIES(include)

FIND_LIBRARY(GLOG "glog")
FIND_PACKAGE(Boost COMPONENTS REQUIRED system thread)
LIST(APPEND SERVER_SRC_LIST src/fxserver.cc 
                            src/fxbuffer.cc 
                            src/fxtimer.cc 
                            src/fxutils.cc
                            src/fx_timer_mgr.cc
                            src/fxconnection.cc
                            src/main.cc
                            )

ADD_EXECUTABLE( ${BIN} ${SERVER_SRC_LIST} )
TARGET_LINK_LIBRARIES( ${BIN} 
                        ${GLOG} 
                        ${Boost_LIBRARIES}
                        )
