INCLUDE(CheckCXXCompilerFlag)
CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
SET(CMAKE_C_COMPILER "gcc")
SET(CMAKE_CXX_COMPILER "g++")
PROJECT( fx )

SET(CMAKE_CXX_FLAGS "-Wall -Wextra -Werror -Wno-unused-variable -Wno-invalid-offsetof -Wno-strict-aliasing" )
SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} -ggdb")
SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} -DNDEBUG -O3" )

CHECK_CXX_COMPILER_FLAG("-Wno-unused-local-typedefs" support)
IF(support)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-local-typedefs")
ENDIF()

SET(CMAKE_FIND_LIBRARY_PREFIXES "lib")
SET(CMAKE_FIND_LIBRARY_SUFFIXES ".so" ".a")

SET(LIBNAME "fx")

INCLUDE_DIRECTORIES(/opt/include)
INCLUDE_DIRECTORIES(include)

FIND_LIBRARY(GLOG "glog")
FIND_PACKAGE(Boost COMPONENTS REQUIRED system thread)
LIST(APPEND FX_SRC_LIST 
        src/fx_base.cc
        src/fx_socket_op.cc
        src/fx_buffer.cc
        src/fx_net_address.cc
        src/fx_channel.cc
        src/fx_poller.cc
        src/fx_acceptor.cc
        src/fx_connector.cc
        src/fx_tcp_connection.cc
        src/fx_tcp_client.cc
        src/fx_tcp_server.cc
        src/fx_timer_wheel.cc
        src/fx_event_loop.cc
        src/fx_event_loop_thread.cc
        src/fx_event_loop_thread_pool.cc
        #src/main.cc
    )

ADD_LIBRARY( ${LIBNAME} STATIC ${FX_SRC_LIST} )
ADD_SUBDIRECTORY( examples )
