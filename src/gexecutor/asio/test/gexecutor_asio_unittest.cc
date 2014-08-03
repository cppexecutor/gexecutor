/**
 * gexecutor_unittest.cc
 *  Copyright Admin Cppexecutor, 2013
 *  Created on: Jun 9, 2013
 *      Author: cppexecutor@gmail.com
 */

#include <gtest/gtest.h>
#include "gexecutor_asio_unittest.h"
#include <iostream>
#include <glog/logging.h>
#include <gexecutor.h>
#include <gasync_executor_asio.h>
#include <gsync_executor_asio.h>
#include <set>
#include <event2/event.h>
#include <pthread.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <ctype.h>
#include <gflags/gflags.h>
#include <gexecutor_service.h>
#include <deferred_task.h>

using namespace std;
DEFINE_int64(timeout_us, 100000, "timeout value in usecs for events");
DEFINE_int64(timeout_s, 0, "timeout value in secs");
DEFINE_int32(num_threads, 2, "Number of threads for running tests");
DEFINE_int32(max_events, 3, "Max Number of events for tests");


class GETestEnvironment: public testing::Environment {
public:
    virtual void SetUp() {
    }

    virtual void TearDown() {
    }
};

int main(int argc, char *argv[]) {
    int result;

    google::InitGoogleLogging(argv[0]);

    FLAGS_logtostderr = 0;

    testing::InitGoogleTest(&argc, argv);

    char curr_dir[1024];
    getcwd(curr_dir, sizeof(curr_dir));
    cout << "current dir" << curr_dir << endl;
    google::ParseCommandLineFlags(&argc, &argv, true);
    FLAGS_logbufsecs = 0;
    testing::AddGlobalTestEnvironment(new GETestEnvironment());

    result = RUN_ALL_TESTS();

    return result;
}

/**
 * Tests contructors
 */
TEST_F(GExecutorAsioTest, InitializeConstructorsSmoke) {
    GTaskQSharedPtr taskq(new GTaskQ());

    GExecutor *async_engine =
            new GAsyncExecutorAsio(io_service_, taskq);

//    GExecutor *sync_engine =
//            new GSyncExecutorAsio(taskq);

    ASSERT_TRUE(async_engine != NULL);
//    ASSERT_TRUE(sync_engine != NULL);

    delete async_engine;
//    delete sync_engine;
    //delete taskq;
}


/**
 * Tests contructors
 */


int deferred_hello() {
    GEXECUTOR_LOG(GEXECUTOR_TRACE)
            << "Deferred: "
            << __FUNCTION__
            << " Hello \n";
    return 42;
}


TEST_F(GExecutorAsioTest, TestSingleTask) {
    GExecutorServiceAsio *asio_executors = new GExecutorServiceAsio(true);

    GExecutorSharedPtr async_engine =
            asio_executors->gexecutor(GExecutorServiceBase::kDefaultExecutorId);

    ASSERT_TRUE(async_engine != NULL);
    GTaskQSharedPtr p_resp_taskq = async_engine->taskq();
    boost::shared_ptr<DeferredTask<int>> p_task(
            new DeferredTask<int>(p_resp_taskq, deferred_hello));
    async_engine->EnQueueTask(p_task);
    for (auto index =0; index < 10; index++) {
        asio_executors->io_service().run_one();
    }
    //delete asio_executors;
}

void deferred_hello_cb(int result) {
    GEXECUTOR_LOG(GEXECUTOR_TRACE)
            << "Deferred: "
            << __FUNCTION__
            << " Hello: "
            << " result " << result << std::endl;
    return;

}

TEST_F(GExecutorAsioTest, TestSyncEngine) {
    GExecutorServiceAsio *asio_executors = new GExecutorServiceAsio(true);
    GExecutorSharedPtr async_engine =
            asio_executors->gexecutor(GExecutorServiceBase::kDefaultExecutorId);

    GExecutorSharedPtr sync_engine = asio_executors->CreateSyncExecutor("SYNC", 10);

    ASSERT_TRUE(sync_engine != NULL);
    ASSERT_TRUE(async_engine != NULL);
    GTaskQSharedPtr p_resp_taskq = async_engine->taskq();
    boost::shared_ptr<DeferredTask<int>> p_task(
            new DeferredTask<int>(p_resp_taskq, deferred_hello));

    p_task->set_callback(deferred_hello_cb);
    sync_engine->EnQueueTask(p_task);
    for (auto index =0; index < 10; index++) {
        asio_executors->io_service().run_one();
    }
    //delete asio_executors;
}

