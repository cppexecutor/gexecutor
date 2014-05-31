cmake_minimum_required (VERSION 2.6)
set (CMAKE_CXX_COMPILER  g++)
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -std=c++11 -Wall -Werror")

project(http_hybrid_server)

include_directories(${PROJECT_SOURCE_DIR}/../../)
set(HHS_MODULE_NAME "http_hybrid_server")
set(HHS_MODULE_SRCS http_hybrid_server.cc main.cc)

add_executable(${HHS_MODULE_NAME} ${HHS_MODULE_SRCS})
	
target_link_libraries(${HHS_MODULE_NAME} event gflags glog pthread dl 
    gexecutor)
