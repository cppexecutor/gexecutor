cmake_minimum_required (VERSION 2.6)
set (CMAKE_CXX_COMPILER  g++)
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -std=c++11 -Wall -Werror")

project(simple_http_server)

include_directories(${PROJECT_SOURCE_DIR}/../../)
set(SHS_MODULE_NAME "simple_http_server")
set(SHS_MODULE_SRCS simple_http_server.cc main.cc)

add_executable(${SHS_MODULE_NAME} ${SHS_MODULE_SRCS})

target_link_libraries(${SHS_MODULE_NAME} event gflags glog pthread dl 
    gexecutor)
