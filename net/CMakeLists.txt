CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

include_directories(${PROJECT_SOURCE_DIR}/base)
include_directories(${PROJECT_SOURCE_DIR}/net)

list(APPEND net_srcs channel.cc poller_impl.cc event_loop.cc timer_manager_impl.cc net_address.cc)
list(APPEND net_srcs udp/udp_listener.cc udp/udp_server.cc)

add_library("fxnet" STATIC ${net_srcs})
target_link_libraries("fxnet" "fxbase")
