CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

SET(BIN "gtest-all")

INCLUDE_DIRECTORIES(/opt/include)
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/include)
include_directories(${PROJECT_SOURCE_DIR}/base/bus)

FIND_LIBRARY(GTEST "gtest")
FIND_LIBRARY(PTHREAD "pthread" )
FIND_LIBRARY(GLOG "glog")
#FIND_PACKAGE(Boost COMPONENTS REQUIRED system thread)
LIST(APPEND SRC_LIST 
            gtest-all.cc
            NetAddress/main.cc
            Buffer/main.cc
            Slice/main.cc
            RingBuffer/main.cc
            ProcessBus/main.cc
            ProcessBusManager/main.cc
    )

ADD_EXECUTABLE( ${BIN} ${SRC_LIST} )
TARGET_LINK_LIBRARIES( ${BIN} "fx" ${GTEST} ${PTHREAD} ${GLOG} "fxbase")
ADD_TEST("gtest-all" ${BIN})
