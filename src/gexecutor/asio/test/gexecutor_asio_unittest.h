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
#include <asio/io_service.hpp>


class GExecutorAsioTest : public testing::Test {
public:
    void CheckForEventExecution(std::set<bool *> events_list) {
        return;
    }

protected:
    boost::asio::io_service io_service_;
    GExecutorServiceAsio g_svc_;
    virtual void SetUp() {
    }
    virtual void TearDown() {
    }
private:
};


#endif /* GEXECUTOR_UNITTEST_ASIO_H_ */
