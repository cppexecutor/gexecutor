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


/**
 * Tests contructors
 */
TEST_F(GExecutorAsioTest, InitializeConstructorsSmoke) {
    GTaskQSharedPtr taskq(new GTaskQ());

    GExecutor *async_engine =
            new GAsyncExecutorAsio(event_base_, taskq);

    GExecutor *sync_engine =
            new GSyncExecutorAsio(taskq);

    ASSERT_TRUE(async_engine != NULL);
    ASSERT_TRUE(sync_engine != NULL);

    delete async_engine;
    delete sync_engine;
    //delete taskq;
}

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

