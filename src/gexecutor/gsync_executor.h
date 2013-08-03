/**
 * gsync_executor.h
 *  Copyright Admin Cppexecutor, 2013
 *  Created on: Jun 18, 2013
 *      Author: cppexecutor@gmail.com
 */

#ifndef GSYNC_EXECUTOR_H_
#define GSYNC_EXECUTOR_H_
#include <event2/event.h>
#include "gexecutor.h"
#include "gtaskq.h"
#include <set>

/**
 *  \description
 *  This implements a synchronous executor engine where tasks are processed
 *  by individual worker threads using unbounded task queue.
 *
 */
#include "gsync_worker_thread.h"

class GSyncWorkerThread;

class GSyncExecutor: public GExecutor {
public:
    GSyncExecutor(GExecutorType type,
                  GTaskQ* taskq,
                  size_t num_workers = 4);

    virtual ~GSyncExecutor();
    virtual gerror_code_t EnQueueTask(GTask *task);
    virtual gerror_code_t Shutdown();
    virtual GTaskQ* taskq() {
        return p_taskq_;
    }
    /**
     * Creates workers in detached state and setup to work on the task queue.
     * @return
     */
    gerror_code_t Initialize();
private:
    GTaskQ *p_taskq_;
    size_t num_workers_;
    std::set<GSyncWorkerThread *> workers_;
    //struct event_base *sync_event_base_;
};

#endif /* GSYNC_EXECUTOR_H_ */
