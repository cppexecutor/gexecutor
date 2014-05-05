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
#include <event2/event.h>
#include <event2/http.h>
#include <event2/buffer.h>
#include <event2/util.h>
#include <event2/keyvalq_struct.h>


class HTTPHybridServer {
public:
    HTTPHybridServer(uint32_t num_sync_workers);
    virtual ~HTTPHybridServer();
    int32_t Initialize();
    int32_t Run();
private:
    const char* docroot_;
    uint32_t num_sync_workers_;
    struct event_base *event_base_;
    struct evhttp *evhttp_;
    struct evhttp_bound_socket *socket_handle_;
};

DECLARE_int32(num_sync_workers);
#define HTTPD_LOG_TRACE 0
#define HTTPD_LOG_ERROR 1

#ifndef HTTPD_LOG
#define HTTPD_LOG(level) \
    std::cout  \
        << "HTTPD:[" << __FUNCTION__ << ":" << __LINE__ << "]"
#endif

#endif /* HTTP_HYBRID_SERVER_H_ */
