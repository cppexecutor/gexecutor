/**
 * gsync_executor.h
 *  Copyright Admin Cppexecutor, 2013
 *  Created on: Jun 18, 2013
 *      Author: cppexecutor@gmail.com
 */

#ifndef GSYNC_EXECUTOR_H_
#define GSYNC_EXECUTOR_H_
#include <event2/event.h>
#include "gexecutor/gexecutor.h"
#include "gexecutor/gtaskq.h"
#include <set>

/**
 *  \description
 *  This implements a synchronous executor engine where tasks are processed
 *  by individual worker threads using unbounded task queue.
 *
 */
#include "gsync_worker_thread.h"

class GSyncWorkerThread;
typedef boost::shared_ptr<GSyncWorkerThread> GSyncWorkerThreadSharedPtr;


class GSyncExecutor: public GExecutor {
public:
    GSyncExecutor(GTaskQSharedPtr taskq,
                  size_t num_workers = 4);

    virtual ~GSyncExecutor();
    virtual gerror_code_t EnQueueTask(GTaskSharedPtr task);
    virtual gerror_code_t Shutdown();
    virtual GTaskQSharedPtr taskq() {
        return p_taskq_;
    }
    /**
     * Creates workers in detached state and setup to work on the task queue.
     * @return
     */
    virtual gerror_code_t Initialize();
private:
    size_t num_workers_;
    std::set<GSyncWorkerThreadSharedPtr> workers_;
    GEXECUTOR_DISALLOW_EVIL_CONSTRUCTORS(GSyncExecutor);
};

typedef boost::shared_ptr<GSyncExecutor> GSyncExecutorSharedPtr;

#endif /* GSYNC_EXECUTOR_H_ */
