INCLUDE(CheckCXXCompilerFlag)
CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(fx_base)

include_directories(/opt/include)

list(APPEND base_srcs bus/ring_buffer.cc 
                      bus/process_bus.cc
                      bus/process_bus_manager.cc
                      pidfile/pidfile.cc
                      )

add_library("fxbase" STATIC ${base_srcs})
