INCLUDE(CheckCXXCompilerFlag)
CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

include_directories(${PROJECT_SOURCE_DIR}/base)

list(APPEND base_srcs system_time.cc mmap_file.cc memory_list.cc slice.cc)
list(APPEND base_srcs container/ring_buffer.cc)
list(APPEND base_srcs bus/process_bus.cc)# bus/process_bus_manager.cc)

add_library("fxbase" STATIC ${base_srcs})
