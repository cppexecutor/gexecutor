/**
 * gsync_worker_thread.h
 *  Copyright Admin Cppexecutor, 2013
 *  Created on: Jul 21, 2013
 *      Author: cppexecutor@gmail.com
 */

#ifndef GSYNC_WORKER_THREAD_H_
#define GSYNC_WORKER_THREAD_H_

#include "gexecutor.h"
#include "gsync_executor.h"
#include "gasync_executor.h"

class GSyncExecutor;

class GSyncWorkerThread {
public:
    GSyncWorkerThread(GSyncExecutor *sync_executor,
                      GTaskQSharedPtr taskq,
                      const std::string& worker_id);
    gerror_code_t Initialize();
    void Shutdown();
    virtual ~GSyncWorkerThread();
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
    GSyncExecutor *sync_executor_;
    GTaskQSharedPtr taskq_;
    pthread_t thread_id_;
    std::string worker_id_;
    struct event_base *event_base_;
    GAsyncExecutor *async_executor_;
};

#endif /* GSYNC_WORKER_THREAD_H_ */
