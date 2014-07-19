cmake_minimum_required (VERSION 2.6)
set (CMAKE_CXX_COMPILER  g++)
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -std=c++11 -Wall -Werror")
set (GEXECUTOR_VERSION 1.0.0)

project(gexecutorasiolib)

include_directories(${PROJECT_SOURCE_DIR}/../)
include_directories(${PROJECT_SOURCE_DIR}/../../)

set (GEXECUTOR_SOURCES gasync_executor_asio.cc gsync_executor_asio.cc 
		gsync_worker_thread_asio.cc gexecutor_service_asio.cc
		../gexecutor_service_base.cc)

add_library(gexecutor-asio SHARED ${GEXECUTOR_SOURCES})
target_link_libraries(gexecutor-asio event pthread gexecutor-common)
set_target_properties(gexecutor-asio PROPERTIES VERSION ${GEXECUTOR_VERSION} SOVERSION 1)

install (FILES gexecutor_service_asio.h gsync_executor_asio.h 
        gasync_executor_asio.h
        DESTINATION include/gexecutor/asio)

install(TARGETS gexecutor-asio
            RUNTIME DESTINATION bin
            LIBRARY DESTINATION lib
            ARCHIVE DESTINATION lib/static)

add_subdirectory(test)