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
    GAsyncExecutor(struct event_base *event_base,
                   GTaskQSharedPtr taskq);
    virtual ~GAsyncExecutor();
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
    virtual gerror_code_t Shutdown() {
        if (p_taskq_ev_) {
            event_del(p_taskq_ev_);
            event_free(p_taskq_ev_);
            p_taskq_ev_ = 0;
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
        p_timer_ev_ = 0;
    }

protected:
    struct event_base *event_base_;
private:
    struct event *p_taskq_ev_;
    struct event *p_timer_ev_;
    struct timeval taskq_timeout_;
    struct timeval timer_timeout_;
    GEXECUTOR_DISALLOW_EVIL_CONSTRUCTORS(GAsyncExecutor);
};
typedef boost::shared_ptr<GAsyncExecutor> GAsyncExecutorSharedPtr;


#endif /* GASYN_CEXECUTOR_H_ */
