/**
 * gexecutor_unittest_asio.h
 *  Copyright Gaurav Rastogi, 2014
 *  Created on: July 19, 2013
 *      Author: grastogi
 */

#ifndef GEXECUTOR_UNITTEST_ASIO_H_
#define GEXECUTOR_UNITTEST_ASIO_H_
#include <gexecutor_service_asio.h>
#include <gasync_executor_asio.h>
#include <gsync_executor_asio.h>


class GExecutorAsioTest : public testing::Test {
public:
    void CheckForEventExecution(std::set<bool *> events_list) {
        return;
    }

protected:
    struct event_base *event_base_;
    GExecutorServiceAsio g_svc_;
    virtual void SetUp() {
        event_base_ = event_base_new();
        if (!event_base_) {
            ASSERT_TRUE(event_base_ != NULL);
            GEXECUTOR_LOG(GEXECUTOR_TRACE) << "Event Base null\n";
            return;
        }
    }
    virtual void TearDown() {
        if (event_base_) {
            free(event_base_);
        }
    }
private:
};


#endif /* GEXECUTOR_UNITTEST_ASIO_H_ */
