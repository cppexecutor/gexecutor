/**
 * deferred_task.h
 *  Copyright Gaurav Rastogi, 2013
 *  Created on: May 29, 2014
 *      Author: grastogi
 */

#ifndef DEFERRED_TASK_H_
#define DEFERRED_TASK_H_
#include "gexecutor/gexecutor_common.h"
#include "gexecutor/gtaskq.h"
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <error.h>

/**
 * Template to create Deferred task which takes the callback function
 * executed as a deferred task in a remote executor.
 *
 * It also supports a success callback and error callback that are invoked
 * in the originator thread / executor.
 *
 * Eg. Let us say there is a function print_hello that needs to be executed
 * in a remote task in a synchronous executor
 *
 * GExecutorSharedPtr sync_executor = executor_svc.gexecutor(
 *           executor_svc.kDefaultExecutorId);
 * GTaskQSharedPtr taskq = sync_executor->taskq();
 * boost::shared_ptr<DeferredTask<void>> d(
 *          new DeferredTask<void>(taskq, print_hello);
 * // attach callback when task print_hello was successful
 * d.set_callback(print_hello_done);
 * // attach callback when task print_hello failed.
 * d.set_errback(print_hello_failed);
 * sync_executor->EnQueueTask(d);
 */

template <class Rtype>
class DeferredTask: public GTask {
public:
    /**
     * Constructor for the Deferred Task which accepts the response queue and
     * task_func that returns Rtype result.
     * @param response_queue_
     * @param task_fn
     */
    DeferredTask(GTaskQSharedPtr response_queue_,
                 boost::function<Rtype()> task_fn)
        : GTask(response_queue_), task_fn_(task_fn), callback_(NULL),
          errback_(NULL) {
    }
    virtual ~DeferredTask() {
    }
    virtual gerror_code_t Execute();
    /**
     * Sets the callback that would be called in the executor that is serving
     * resposne_queue_
     * @param callback
     */
    void set_callback(boost::function<void(Rtype)> callback) {
        callback_ = callback;
    }
    /**
     * Sets the error callback called in the executor that is serving the
     * response_queue_
     * @param errback
     */
    void set_errback(boost::function<void(const gerror_code_t&)> errback) {
        errback_ = errback;
    }
private:
    boost::function<Rtype()> task_fn_;
    boost::function<void(Rtype)> callback_;
    boost::function<void(const gerror_code_t&)> errback_;
    GEXECUTOR_DISALLOW_EVIL_CONSTRUCTORS(DeferredTask);
};

template<class CbArg>
void deferred_cb(boost::function<void(CbArg)> callback_, CbArg result) {
    callback_(result);
}

/**
 * Template specialization for the callbacks that do not return any result i.e.
 * Rtype = void
 */
template <>
class DeferredTask<void>: public GTask {
public:
    DeferredTask(GTaskQSharedPtr response_queue_,
                 boost::function<void()> task_fn)
        : GTask(response_queue_), task_fn_(task_fn), callback_(NULL),
          errback_(NULL) {
    }
    virtual ~DeferredTask() {
    }
    virtual gerror_code_t Execute() {
        try {
            task_fn_();
            if (callback_ == NULL) {
                return 0;
            }
            GTaskSharedPtr p_task(
                    new DeferredTask<void>(exec_task_q_, callback_));
            resp_task_q_->EnqueueGTask(p_task);
        }
        catch(...) {
            if (errback_ == NULL) {
                return 0;
            }
            GTaskSharedPtr p_task(
                    new DeferredTask<void>(exec_task_q_,
                                           boost::bind(deferred_cb<gerror_code_t>,
                                                       errback_,
                                                       -errno)));
            resp_task_q_->EnqueueGTask(p_task);
        }
        return 0;
    }
    void set_callback(boost::function<void()> callback) {
        callback_ = callback;
    }
    void set_errback(boost::function<void(const gerror_code_t&)> errback) {
        errback_ = errback;
    }
private:
    boost::function<void()> task_fn_;
    boost::function<void()> callback_;
    boost::function<void(const gerror_code_t&)> errback_;
    GEXECUTOR_DISALLOW_EVIL_CONSTRUCTORS(DeferredTask);
};

/**
 * Generic implementation of the Execute virtual function of the GTask
 * Interface.
 * @return
 */
template<class Rtype>
gerror_code_t DeferredTask<Rtype>::Execute() {
    try {
        Rtype result = task_fn_();
        if (callback_ == NULL) {
            return 0;
        }
        GTaskSharedPtr p_task(
                new DeferredTask<void>(exec_task_q_,
                                       boost::bind(deferred_cb<Rtype>,
                                                   callback_,
                                                   result)));
        resp_task_q_->EnqueueGTask(p_task);
    }
    catch(...) {
        if (errback_ == NULL) {
            return 0;
        }
        GTaskSharedPtr p_task(
                new DeferredTask<void>(exec_task_q_,
                                       boost::bind(deferred_cb<gerror_code_t>,
                                                   errback_,
                                                   -errno)));
        resp_task_q_->EnqueueGTask(p_task);
    }
    return 0;
}


#endif /* DEFERRED_TASK_H_ */
