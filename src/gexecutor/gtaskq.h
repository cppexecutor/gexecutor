/**
 * gtaskq.h
 *  Copyright Admin Cppexecutor, 2013
 *  Created on: Jun 23, 2013
 *      Author: cppexecutor@gmail.com
 */

#ifndef GTASKQ_H_
#define GTASKQ_H_
#include "gexecutor/gexecutor_common.h"
#include "gexecutor/gexecutor.h"
#include <queue>
#include <deque>
#include <sys/wait.h>
#include <unistd.h>
#include <glog/logging.h>
#include <assert.h>
#include <pthread.h>

/**
 * This is the task interface that should be extended by overriding the
 * Execute() function. The Execute is called by the executor when it receives
 * a task in its queue.
 *
 */
class GTask : public boost::enable_shared_from_this<GTask> {
public:
    /**
     * Need to initialize the GTask where the response should be sent.
     *
     * @param response_queue_: This is the queue for sending back response
     * for this task
     */
    explicit GTask(GTaskQSharedPtr response_queue_);
    explicit GTask(GTaskQSharedPtr response_queue_,
                   const std::string& debug_str);

    virtual ~GTask();
    /**
     * This is the Task callback that application would like to run in a given
     * executor.
     * @return
     */
    virtual gerror_code_t Execute() {
        GEXECUTOR_LOG(GEXECUTOR_TRACE) <<
                "Default Task Called\n";
        return 0;
    }

    void set_exec_task_q(GTaskQSharedPtr execution_task_q) {
        assert(exec_task_q_ == NULL);
        exec_task_q_ = execution_task_q;
    }

    void set_executor(GExecutor* executor) {
        executor_ = executor;
    }
    virtual const std::string& DebugString() {
        return debug_str_;
    }
protected:
    /**
     * contains the response taskq pointer. This can be accessed by Execute()
     * implementation to send back the response task to the executor that
     * had originated the task.
     */
    GTaskQSharedPtr resp_task_q_;
    GTaskQSharedPtr exec_task_q_;
    GExecutor* executor_;
    std::string debug_str_;
    gerror_code_t error_code_;
private:
    GEXECUTOR_DISALLOW_EVIL_CONSTRUCTORS(GTask);
};

/**
 *
 *
 *
 */

class GTaskQ : public boost::enable_shared_from_this<GTaskQ> {
public:
    GTaskQ();
    virtual ~GTaskQ();
    /**
     *
     * @param task
     * @param ev_base: This is event base of the caller. It is possible that
     * equeue may need to be buffered as signal to the other end has not
     * happened or need to be re-tried.
     * @return
     */
    virtual gerror_code_t Initialize();
    virtual gerror_code_t EnqueueGTask(GTaskSharedPtr task);
    virtual GTaskSharedPtr DequeueGTask();
    virtual int read_fd() {
        return pipefds_[PIPE_FD_READ_INDX];
    }
    virtual int write_fd() {
        return pipefds_[PIPE_FD_WRITE_INDX];
    }
    virtual void set_gexecutor(GExecutor* executor_ctx) {
        executor_ctx_ = executor_ctx;
    }
    int64_t num_enqueue() {
        return num_enqueue_;
    }

    int64_t num_dequeue() {
        return num_dequeue_;
    }

protected:
    virtual gerror_code_t NotifyQ(size_t num_notifn,
                                  ssize_t *num_notified);
    size_t num_notifn_to_write(size_t num_new_notifn);
    int64_t num_enqueue_;
    int64_t num_dequeue_;
private:
    size_t max_outstanding_notifn_;
    enum PIPE_FD_INDX {
        PIPE_FD_READ_INDX = 0,
        PIPE_FD_WRITE_INDX = 1,
        PIPE_NUM_FDS = PIPE_FD_WRITE_INDX
    };
    std::deque<GTaskSharedPtr> task_queue_;
    GExecutor* executor_ctx_;
    size_t num_outstanding_notifn_;
    int pipefds_[PIPE_NUM_FDS];
    char *notfn_buffer_;
    pthread_mutex_t q_lock_;
    GEXECUTOR_DISALLOW_EVIL_CONSTRUCTORS(GTaskQ);
};

//typedef boost::shared_ptr<GTaskQ> GTaskQSharedPtr;

#endif /* GTASKQ_H_ */
