/**
 * http_hybrid_server.h
 *  Copyright Gaurav Rastogi, 2013
 *  Created on: May 4, 2014
 *      Author: grastogi
 */

#ifndef HTTP_HYBRID_SERVER_H_
#define HTTP_HYBRID_SERVER_H_
#include <stdint.h>
#include <gtest/gtest.h>
#include <iostream>
#include <glog/logging.h>
#include <gflags/gflags.h>

class HTTPHybridServer {
public:
    HTTPHybridServer(uint32_t num_sync_workers);
    virtual ~HTTPHybridServer();
    void Initialize();
    void Run();
private:
    uint32_t num_sync_workers_;
    struct event_base *event_base_;
};

DECLARE_int32(num_sync_workers);

#endif /* HTTP_HYBRID_SERVER_H_ */
