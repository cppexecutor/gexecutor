cmake_minimum_required (VERSION 2.6)
set (CMAKE_CXX_COMPILER  g++)
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -std=c++11 -Wall -Werror")

project(executor)

include_directories(${PROJECT_SOURCE_DIR}/../)
set (GEXECUTOR_SOURCES gexecutor.cc gasync_executor.cc gsync_executor.cc 
		gtaskq.cc gsync_worker_thread.cc)

add_library(gexecutor SHARED ${GEXECUTOR_SOURCES})
target_link_libraries(gexecutor event pthread)

add_subdirectory(test)

