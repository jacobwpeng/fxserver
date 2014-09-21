cmake_minimum_required(VERSION 2.6)
project(fx_examples)

include_directories(${PROJECT_SOURCE_DIR}/base/bus)

#add_subdirectory(server)
#add_subdirectory(client)
#add_subdirectory(object_pool)
#add_subdirectory(httpserver)
add_subdirectory(fxconnd bin)
