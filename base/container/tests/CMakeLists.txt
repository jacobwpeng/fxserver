cmake_minimum_required(VERSION 2.6)

set(bin "gtest-all")

include_directories(${PROJECT_SOURCE_DIR})

find_library(gtest "gtest")
find_library(pthead "pthread" )
#FIND_PACKAGE(Boost COMPONENTS REQUIRED system thread)
list(APPEND base_test_srcs
            gtest-all.cc
            memory_list_unittest.cc
            list_unittest.cc
            rbtree_unittest.cc
    )

add_executable( ${bin} ${base_test_srcs} )
target_link_libraries( ${bin} "fxbase" ${gtest} ${pthead})
add_test("gtest-all" ${bin})
