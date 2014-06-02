/**
 * gexecutor.h
 *  Copyright Admin Cppexecutor, 2013
 *  Created on: Jun 9, 2013
 *      Author: cppexecutor@gmail.com
 */

#ifndef GEXECUTOR_H_
#define GEXECUTOR_H_

#include <event2/event.h>
#include <string>
#include <set>
#include "gexecutor/gtaskq.h"
#include "gexecutor/gexecutor_common.h"
#include <glog/logging.h>
#include <unordered_map>
/**
 * \description:
 *
 * There are two types of execution paradigms
 * 1. Asynchronous (typically using non-blocking I/O)
 * 2. Synchronous (typically using blocking I/O)
 *
 *
 * What is an executor?
 * Following the interface from Java, an Executor is simply a service that
 * has a queue to accept tasks. If an executor is async then it would run
 * the task in async event loop.
 *
 * If the executor is a synchronous then it would have a pool of threads that
 * are all working off the same queue of tasks. One of the threads in the
 * queue would pick up the task and execute it.
 *
 */

class GExecutor : public boost::enable_shared_from_this<GExecutor> {
 public:
    enum GExecutorType {
        ASYNC = 0,
        SYNC
    };

    /**
     *
     * @param type
     * @param event_base
     * @param taskq: This needs to be created before threads are created.
     */

    virtual ~GExecutor();

    virtual gerror_code_t EnQueueTask(GTaskSharedPtr task) = 0;

    /**
     * Returns the underlying taskq for this executor. This task queue can be
     * used to enqueue tasks for this execution block.
     * @return
     */
    virtual GTaskQSharedPtr taskq() {
        return p_taskq_;
    }
    /**
     * signals close of the taskq and shuts down the worker threads for the
     * synchronous executor. In case of async executor it woudl leave the
     * application handle the exit of the thread/process.
     * @return
     */
    virtual gerror_code_t Shutdown() = 0;

    /**
     * Returns type of the execution block
     * @return
     */
    GExecutorType type() const {
        return gexec_type_;
    }

    virtual gerror_code_t Initialize() = 0;

 protected:
    // struct event_base *event_base_;
    GExecutor(GExecutorType type,
              GTaskQSharedPtr taskq);
    GExecutorType gexec_type_;
    GTaskQSharedPtr p_taskq_;
    uint64_t num_enqueue_;
    uint64_t num_dequeue_;
    uint64_t num_task_with_response_;
    uint64_t rate_of_dequeue_in_100secs;
    uint64_t max_rate_of_dequeue;
    uint64_t rate_of_enqueue_in_100secs;
    uint64_t max_rate_of_enqueue;

 private:
    GEXECUTOR_DISALLOW_EVIL_CONSTRUCTORS(GExecutor);
};


#endif /* GEXECUTOR_H_ */
