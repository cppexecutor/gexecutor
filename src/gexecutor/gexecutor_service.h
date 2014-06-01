/**
 * gexecutor_server.h
 *  Copyright Admin Cppexecutor, 2013
 *  Created on: Jun 23, 2013
 *      Author: cppexecutor@gmail.com
 */

#ifndef GEXECUTOR_SERVER_H_
#define GEXECUTOR_SERVER_H_
#include "gexecutor/gexecutor.h"
#include <unordered_map>

/**
 *  A simple reactor can be created as
 *  GExecutorService executor_svc(true);
 *  struct event_base *event_base = executor_svc.event_base();
 *  executor_svc.run();
 *
 *  DeferredTask can be created as
    GExecutorSharedPtr async_executor = executor_svc.gexecutor(
 *      executor_svc.kDefaultExecutorId);
 *  GTaskQSharedPtr taskq = async_executor->taskq();

    boost::shared_ptr<DeferredTask<void>> d(
            new DeferredTask<void>(
                    taskq, task_done_callback);

    d.set_callback(callback);
    async_executor_->EnQueueTask(d);
 *
 *
 */

class GExecutorService {
public:
    static const std::string kDefaultExecutorId;
    GExecutorService(bool enable_default_async_executor=false);
    virtual ~GExecutorService();
    gerror_code_t ShutdownExecutor(const std::string& gexecutor_id);
    GExecutorSharedPtr CreateAsyncExecutor(
            const std::string& executor_id,
            GTaskQSharedPtr p_taskq,
            struct event_base *async_event_base);

    GExecutorSharedPtr CreateSyncExecutor(
            const std::string& executor_id,
            size_t num_default_threads);

    GExecutorSharedPtr gexecutor(const std::string& gexecutor_id =
            kDefaultExecutorId);
    void run() {
        event_base_dispatch(default_async_event_base_);
    }
    struct event_base * event_base() {
        return default_async_event_base_;
    }

private:
    pthread_mutex_t gexecutor_svc_lock_;
    struct event_base *default_async_event_base_;
    GTaskQSharedPtr default_taskq_;
    GExecutorSharedPtr default_async_executor_;
    std::unordered_map<std::string, GExecutorSharedPtr> gexecutor_map_;
    GEXECUTOR_DISALLOW_EVIL_CONSTRUCTORS(GExecutorService);
};

#endif /* GEXECUTOR_SERVER_H_ */
