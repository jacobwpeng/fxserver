CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT( fxserver )

SET(CMAKE_C_COMPILER "gcc")
SET(CMAKE_CXX_COMPILER "g++")

SET(CMAKE_CXX_FLAGS "-Wall -Werror")

SET(CMAKE_FIND_LIBRARY_PREFIXES "lib")
SET(CMAKE_FIND_LIBRARY_SUFFIXES ".so" ".a")

SET(BIN "test_fxserver")

INCLUDE_DIRECTORIES(/opt/include)
INCLUDE_DIRECTORIES(include)

FIND_LIBRARY(GLOG "glog")
#FIND_PACKAGE(Boost COMPONENTS REQUIRED system thread)
LIST(APPEND SERVER_SRC_LIST src/fxserver.cc src/fxbuffer.cc src/main.cc)

ADD_EXECUTABLE( ${BIN} ${SERVER_SRC_LIST} )
TARGET_LINK_LIBRARIES( ${BIN} 
                        ${GLOG} 
                        #${Boost_LIBRARIES}
                        )
