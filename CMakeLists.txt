CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(fx)

SET(CMAKE_CXX_FLAGS "-Wall -Wextra -Werror -Wno-unused-variable -Wno-unused-private-field" )
SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} -ggdb")
SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} -DNDEBUG -O3" )

SET(CMAKE_FIND_LIBRARY_PREFIXES "lib")
SET(CMAKE_FIND_LIBRARY_SUFFIXES ".so" ".a")

SET(LIBNAME "fx")

include_directories(/opt/include)
include_directories(include)
include_directories(base/bus)
include_directories(base/pidfile)

FIND_LIBRARY(GLOG "glog")
FIND_PACKAGE(Boost COMPONENTS REQUIRED system thread)
LIST(APPEND FX_SRC_LIST 
        src/fx_base.cc
        src/fx_slice.cc
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
    )

ADD_LIBRARY( ${LIBNAME} STATIC ${FX_SRC_LIST} )
add_subdirectory(base)
ADD_SUBDIRECTORY(examples)
ENABLE_TESTING()
ADD_SUBDIRECTORY(tests)
