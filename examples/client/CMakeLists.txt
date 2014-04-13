CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

SET(BIN "fxclient")

INCLUDE_DIRECTORIES(/opt/include)
INCLUDE_DIRECTORIES(../../include)

FIND_LIBRARY(GLOG "glog")
FIND_PACKAGE(Boost COMPONENTS REQUIRED system thread)
LIST(APPEND CLIENT_SRC_LIST main.cc
    )

ADD_EXECUTABLE( ${BIN} ${CLIENT_SRC_LIST} )
TARGET_LINK_LIBRARIES( ${BIN} "fx"
                        ${GLOG} 
                        ${Boost_LIBRARIES}
                        )
