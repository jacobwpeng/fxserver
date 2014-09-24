cmake_minimum_required(VERSION 2.6)

set(BIN "gtest-all")

include_directories(${PROJECT_SOURCE_DIR}/base)
include_directories(${PROJECT_SOURCE_DIR}/net)

find_library(GTEST "gtest")
find_library(PTHREAD "pthread" )
find_library(GLOG "glog")
list(APPEND SRC_LIST 
            gtest-all.cc
            slice_unittest.cc
            ringbuffer_unittest.cc
            processbus_unittest.cc
            timermanager_unittest.cc
            netaddress_unittest.cc
    )

add_executable(${BIN} ${SRC_LIST})
target_link_libraries(${BIN} ${GTEST} ${PTHREAD} ${GLOG} "fxbase" "fxnet")
add_test("gtest-all" ${BIN})
