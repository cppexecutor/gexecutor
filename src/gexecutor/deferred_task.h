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


template <class Rtype>
class DeferredTask: public GTask {
public:
    DeferredTask(GTaskQSharedPtr response_queue_,
                 boost::function<Rtype()> task_fn)
        : GTask(response_queue_), task_fn_(task_fn), callback_(NULL),
          errback_(NULL) {
    }
    virtual ~DeferredTask() {
    }
    virtual gerror_code_t Execute();
    void set_callback(boost::function<void(Rtype)> callback) {
        callback_ = callback;
    }
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
