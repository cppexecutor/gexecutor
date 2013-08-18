/**
 * gasyn_cexecutor.h
 *  Copyright Admin Cppexecutor, 2013
 *  Created on: Jun 9, 2013
 *      Author: cppexecutor@gmail.com
 */

#ifndef GASYN_CEXECUTOR_H_
#define GASYN_CEXECUTOR_H_

#include "gexecutor/gexecutor.h"

class GAsyncExecutor : public GExecutor {
public:
    GAsyncExecutor(GExecutorType type, struct event_base *event_base,
                   GTaskQ *taskq);
    virtual ~GAsyncExecutor();
    virtual gerror_code_t Initialize();
    virtual gerror_code_t EnQueueTask(GTask *task);
//    virtual gerror_code_t EnQueueTask(GTask *task,
//                                      GExecutor* task_originator_ctx);
    /**
     *  This is used
     */
    virtual GTaskQ* taskq() {
        return p_taskq_;
    }
    virtual gerror_code_t Shutdown() {
        if (p_taskq_ev_) {
            event_del(p_taskq_ev_);
            event_free(p_taskq_ev_);
            p_taskq_ev_ = NULL;
        }
        StopTimer();
        return 0;
    }

    virtual void StopTimer() {
        if (!p_timer_ev_) {
            return;
        }
        event_del(p_timer_ev_);
        //event_free(p_timer_ev_);
        p_timer_ev_ = NULL;
    }
protected:
    struct event_base *event_base_;
private:
    /**
     * Async executor may have its own event base
     *
     */
    GTaskQ *p_taskq_;
    /**
     * TaskQ may be shared across multiple executors for reading. So, it
     * not part of the taskq data structure.
     */
    struct event *p_taskq_ev_;
    struct event *p_timer_ev_;
    struct timeval taskq_timeout_;
    struct timeval timer_timeout_;

};

#endif /* GASYN_CEXECUTOR_H_ */
