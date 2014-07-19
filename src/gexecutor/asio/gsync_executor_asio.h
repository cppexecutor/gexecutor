/**
 * gsync_executor.h
 *  Copyright Admin Cppexecutor, 2013
 *  Created on: Jun 18, 2013
 *      Author: cppexecutor@gmail.com
 */

#ifndef GSYNC_EXECUTOR_ASIO_H_
#define GSYNC_EXECUTOR_ASIO_H_
#include <event2/event.h>
#include "gexecutor/gexecutor.h"
#include "gexecutor/gtaskq.h"
#include <set>

/**
 *  \description
 *  This implements a synchronous executor engine where tasks are processed
 *  by individual worker threads using unbounded task queue.
 *
 *  Following example shows simple steps to create a Synchronous Executor
 *  with worker threads.
 *  // create taskq where all the synchronous workers woudl listen
 *  GTaskQSharedPtr sync_taskq(new GTaskQ());
 *  sync_taskq->Initialize();
 *  GSyncExecutorAsio *sync_engine =
 *           new GSyncExecutorAsio(sync_taskq);
 *  // Now sync engine is ready for events
 *  Send tasks to the sync engine
 *
 *  boost::shared_ptr<DeferredTask<void>> d(
 *          new DeferredTask<void>(taskq, print_hello);
 *  // attach callback when task print_hello was successful
 *  d.set_callback(print_hello_done);
 *  // attach callback when task print_hello failed.
 *  d.set_errback(print_hello_failed);
 *  sync_executor->EnQueueTask(d);
 */
#include "gsync_worker_thread_asio.h"

class GSyncWorkerThreadAsio;
typedef boost::shared_ptr<GSyncWorkerThreadAsio> GSyncWorkerThreadAsioSharedPtr;


class GSyncExecutorAsio: public GExecutor {
public:
    /**
     * Create Sync Executor that extends the GExecutor
     * @param taskq taskq where all the workers would wait for notification
     * to pick up and execute tasks in synchronous manner.
     * @param num_workers number of workers. Currently it is static. In future
     * can build a mechanism to increase or decrease the number of workers
     * based on the load.
     */
    GSyncExecutorAsio(GTaskQSharedPtr taskq,
                      size_t num_workers = 4);

    virtual ~GSyncExecutorAsio();
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
    std::set<GSyncWorkerThreadAsioSharedPtr> workers_;
    GEXECUTOR_DISALLOW_EVIL_CONSTRUCTORS(GSyncExecutorAsio);
};

typedef boost::shared_ptr<GSyncExecutorAsio> GSyncExecutorAsioSharedPtr;

#endif /* GSYNC_EXECUTOR_ASIO_H_ */
