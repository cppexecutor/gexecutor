/**
 * gsync_worker_thread.h
 *  Copyright Admin Cppexecutor, 2013
 *  Created on: Jul 21, 2013
 *      Author: cppexecutor@gmail.com
 */

#ifndef GSYNC_WORKER_THREAD_ASIO_H_
#define GSYNC_WORKER_THREAD_ASIO_H_

#include "gexecutor/gexecutor.h"
#include "gexecutor/asio/gsync_executor_asio.h"
#include "gexecutor/asio/gasync_executor_asio.h"

class GSyncExecutorAsio;

class GSyncWorkerThreadAsio :
        public boost::enable_shared_from_this<GSyncWorkerThreadAsio> {
public:
    GSyncWorkerThreadAsio(GSyncExecutorAsio *sync_executor,
                          GTaskQSharedPtr taskq,
                      const std::string& worker_id);
    gerror_code_t Initialize();
    void Shutdown();
    virtual ~GSyncWorkerThreadAsio();
    struct event_base* event_base() {
        return event_base_;
    }
    GTaskQSharedPtr taskq() {
        return taskq_;
    }
    void SetupEventLoop();
    const pthread_t& id() {
        return thread_id_;
    }
private:
    GSyncExecutorAsio *sync_executor_;
    GTaskQSharedPtr taskq_;
    pthread_t thread_id_;
    std::string worker_id_;
    struct event_base *event_base_;
    GAsyncExecutorAsio *async_executor_;
    GEXECUTOR_DISALLOW_EVIL_CONSTRUCTORS(GSyncWorkerThreadAsio);
};

#endif /* GSYNC_WORKER_THREAD_ASIO_H_ */
