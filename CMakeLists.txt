CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT( fxserver )

SET(CMAKE_C_COMPILER "gcc")
SET(CMAKE_CXX_COMPILER "g++")

SET(CMAKE_CXX_FLAGS "-Wall -Wno-unused-local-typedefs -Werror -O3")

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
