/**
 * gasyn_cexecutor.h
 *  Copyright Admin Cppexecutor, 2013
 *  Created on: Jun 9, 2013
 *      Author: cppexecutor@gmail.com
 */

#ifndef GASYN_EXECUTOR_ASIO_H_
#define GASYN_EXECUTOR_ASIO_H_

#include <event2/event.h>
#include "gexecutor/gexecutor.h"
/**
 *  \description
 *  This implements a asynchronous executor engine where tasks are expected to
 *  be processed in asynchronous manner.
 *
 *  Following example shows simple steps to create a Synchronous Executor
 *  with worker threads.
 *  // create taskq where all the synchronous workers woudl listen
 *  GTaskQSharedPtr async_taskq(new GTaskQ());
 *  async_taskq->Initialize();
 *  GAsyncExecutorAsio *async_engine =
 *           new GAsyncExecutorAsio(async_taskq);
 *  // Now sync engine is ready for events
 *  Send tasks to the sync engine
 *
 *  boost::shared_ptr<DeferredTask<void>> d(
 *          new DeferredTask<void>(taskq, print_hello);
 *  // attach callback when task print_hello was successful
 *  d.set_callback(print_hello_done);
 *  // attach callback when task print_hello failed.
 *  d.set_errback(print_hello_failed);
 *  async_executor->EnQueueTask(d);
 */
class GAsyncExecutorAsio : public GExecutor {
public:
    GAsyncExecutorAsio(struct event_base *event_base,
                   GTaskQSharedPtr taskq);
    virtual ~GAsyncExecutorAsio();
    virtual gerror_code_t Initialize();
    virtual gerror_code_t EnQueueTask(GTaskSharedPtr task);
//    virtual gerror_code_t EnQueueTask(GTask *task,
//                                      GExecutor* task_originator_ctx);
    /**
     *  This is used
     */
    virtual GTaskQSharedPtr taskq() {
        return p_taskq_;
    }
    virtual gerror_code_t Shutdown();

    virtual void StopTimer();

protected:
    struct event_base *event_base_;
private:
    struct event *p_taskq_ev_;
    struct event *p_timer_ev_;
    struct timeval taskq_timeout_;
    struct timeval timer_timeout_;
    GEXECUTOR_DISALLOW_EVIL_CONSTRUCTORS(GAsyncExecutorAsio);
};
typedef boost::shared_ptr<GAsyncExecutorAsio> GAsyncExecutorAsioSharedPtr;


#endif /* GASYN_EXECUTOR_ASIO_H_ */
