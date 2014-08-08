INCLUDE(CheckCXXCompilerFlag)
CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(fx_base)

include_directories(/opt/include)
include_directories(bus/include)

list(APPEND base_srcs bus/src/ring_buffer.cc)

add_library("fxbase" STATIC ${base_srcs})
