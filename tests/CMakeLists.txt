CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

SET(BIN "gtest-all")

INCLUDE_DIRECTORIES(/opt/include)
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/../include)
INCLUDE_DIRECTORIES(/tmp/gtest-1.7.0/include)

FIND_LIBRARY(GTEST "gtest" /tmp/gtest-1.7.0/build)
FIND_LIBRARY(PTHREAD "pthread" )
FIND_LIBRARY(GLOG "glog")
#FIND_PACKAGE(Boost COMPONENTS REQUIRED system thread)
LIST(APPEND SRC_LIST 
            gtest-all.cc
            NetAddress/main.cc
    )

ADD_EXECUTABLE( ${BIN} ${SRC_LIST} )
TARGET_LINK_LIBRARIES( ${BIN} "fx" ${GTEST} ${PTHREAD} ${GLOG})
    #                        ${GLOG} 
    #                        ${Boost_LIBRARIES}
