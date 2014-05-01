cmake_minimum_required (VERSION 2.6)
set (CMAKE_CXX_COMPILER  g++)
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -std=c++11 -Wall -Werror")

include_directories(${PROJECT_SOURCE_DIR}/../)

project(gexecutor_unittest)

enable_testing()

include_directories(${PROJECT_SOURCE_DIR}/../)

set(GRCACHE_UNITTEST_MODULE_NAME "gexecutor_unittest")
set(GRCACHE_UNITTEST_MODULE_SRCS ../gexecutor.cc ../gasync_executor.cc 
	../gsync_executor.cc ../gtaskq.cc ../gsync_worker_thread.cc 
	../gexecutor_service.cc)

add_executable(${GRCACHE_UNITTEST_MODULE_NAME} 
	${GRCACHE_UNITTEST_MODULE_NAME}.cc ${GRCACHE_UNITTEST_MODULE_SRCS})

add_executable(shared_test shared_ptr.cc)

FIND_LIBRARY(GTEST gtest /usr/local/lib)
	
target_link_libraries(${GRCACHE_UNITTEST_MODULE_NAME}
        event gflags glog ${GTEST} pthread dl)


# Integrating the cmake with the unit test program such that make test
# would run the above test case
add_test (${GRCACHE_UNITTEST_MODULE_NAME}_SMOKE
        ${CMAKE_BINARY_DIR}/bin/${GRCACHE_UNITTEST_MODULE_NAME}
        --gtest_filter=*Smoke*
        --logtostderr=0
        --gtest_output=xml:${GRCACHE_UNITTEST_MODULE_NAME}_SMOKE.xml)