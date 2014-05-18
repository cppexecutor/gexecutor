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
#include <gexecutor/gexecutor.h>
#include <gexecutor/gtaskq.h>
#include <gexecutor/gasync_executor.h>
#include <gexecutor/gsync_executor.h>


class HTTPHybridServer {
public:
    HTTPHybridServer(uint32_t num_sync_workers);
    virtual ~HTTPHybridServer();
    int32_t Initialize();
    int32_t Run();
    const std::string& docroot() const {
        return docroot_;
    }
    GTaskQSharedPtr async_taskq() {
        return async_taskq_;
    }
    void handle_request(struct evhttp_request * req);
private:
    std::string docroot_;
    uint32_t num_sync_workers_;
    struct event_base *event_base_;
    struct evhttp *evhttp_;
    struct evhttp_bound_socket *socket_handle_;
    GTaskQSharedPtr async_taskq_;
    GTaskQSharedPtr sync_taskq_;
    GAsyncExecutorSharedPtr async_executor_;
    GSyncExecutorSharedPtr sync_executor_;
};

class HTTPAsyncRequestTask : public GTask {
public:
    HTTPAsyncRequestTask(HTTPHybridServer *http_server,
                         struct evhttp_request *req)
    : GTask(http_server->async_taskq()), http_server_(http_server), req_(req),
      file_path_(""), decoded_path_(NULL), decoded_(NULL), evb_(NULL) {
        return;
    }
    virtual ~HTTPAsyncRequestTask() {
        if (decoded_)
            evhttp_uri_free(decoded_);
        if (decoded_path_)
            free(decoded_path_);
        if (evb_)
            evbuffer_free(evb_);
    }
protected:
    virtual gerror_code_t Execute();
private:
    const HTTPHybridServer *http_server_;
    struct evhttp_request *req_;
    std::string file_path_;
    char* decoded_path_;
    struct evhttp_uri *decoded_;
    struct evbuffer *evb_;
};



DECLARE_int32(num_sync_workers);
#define HTTPD_LOG_TRACE 0
#define HTTPD_LOG_ERROR 1

#ifndef HTTPD_LOG
#define HTTPD_LOG(level) \
    VLOG(level) << "HTTPD:[" << __FUNCTION__ << ":" << __LINE__ << "]"
#endif

#endif /* HTTP_HYBRID_SERVER_H_ */
