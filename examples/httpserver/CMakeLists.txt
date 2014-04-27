CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

SET(BIN "httpserver")

INCLUDE_DIRECTORIES(/opt/include)
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/../../include)
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/httpserver/include)

FIND_LIBRARY(GLOG "glog")
FIND_PACKAGE(Boost COMPONENTS REQUIRED system thread)
LIST(APPEND SRC_LIST 
    src/http_utilities.cc
    src/http_request.cc
    src/http_response.cc
    src/http_codec.cc
    src/http_server.cc
    src/http_module_request_validator.cc
    src/main.cc
    )

ADD_EXECUTABLE( ${BIN} ${SRC_LIST} )
TARGET_LINK_LIBRARIES( ${BIN} "fx"
                        ${GLOG} 
                        ${Boost_LIBRARIES}
                        )
