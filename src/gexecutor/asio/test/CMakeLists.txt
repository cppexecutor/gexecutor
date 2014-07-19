cmake_minimum_required (VERSION 2.6)
set (CMAKE_CXX_COMPILER  g++)
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -std=c++11 -Wall -Werror")

include_directories(${PROJECT_SOURCE_DIR}/../)

project(gexecutor_asio_unittest)

enable_testing()

include_directories(${PROJECT_SOURCE_DIR}/../)

set(GEXECUTOR_UNITTEST_MODULE_NAME "gexecutor_asio_unittest")
set(GEXECUTOR_UNITTEST_MODULE_SRCS ../gasync_executor_asio.cc 
	../gsync_executor_asio.cc ../gsync_worker_thread_asio.cc 
	../gexecutor_service_asio.cc ../../gexecutor_service_base.cc)

add_executable(${GEXECUTOR_UNITTEST_MODULE_NAME} 
	${GEXECUTOR_UNITTEST_MODULE_NAME}.cc ${GEXECUTOR_UNITTEST_MODULE_SRCS})

FIND_LIBRARY(GTEST gtest /usr/local/lib)
	
target_link_libraries(${GEXECUTOR_UNITTEST_MODULE_NAME}
        event gflags glog ${GTEST} pthread dl gexecutor-common)


# Integrating the cmake with the unit test program such that make test
# would run the above test case
add_test (${GEXECUTOR_UNITTEST_MODULE_NAME}_SMOKE
        ${CMAKE_BINARY_DIR}/bin/${GEXECUTOR_UNITTEST_MODULE_NAME}
        --gtest_filter=*Smoke*
        --logtostderr=0
        --gtest_output=xml:${GEXECUTOR_UNITTEST_MODULE_NAME}_SMOKE.xml)
