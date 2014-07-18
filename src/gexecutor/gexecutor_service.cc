/**
 * gexecutor_server.cc
 *  Copyright Admin Cppexecutor, 2013
 *  Created on: Jun 23, 2013
 *      Author: cppexecutor@gmail.com
 */

#include <pthread.h>
#include "gexecutor/gexecutor_service.h"
#include "gasync_executor.h"
#include "gsync_executor.h"


GExecutorService::GExecutorService(bool enable_default_async_executor) :
    GExecutorServiceBase(enable_default_async_executor),
    default_async_event_base_(NULL) {

    if (enable_default_async_executor == false) {
        return;
    }
    default_async_event_base_ = event_base_new();
    default_async_executor_ =
            CreateAsyncExecutor(kDefaultExecutorId, default_taskq_,
                                default_async_event_base_);
}

GExecutorService::~GExecutorService() {
}

GExecutorSharedPtr GExecutorService::CreateAsyncExecutor(
        const std::string& executor_id,
        GTaskQSharedPtr p_taskq,
        struct event_base* async_event_base) {
    if (p_taskq == NULL) {
        GTaskQSharedPtr new_p_taskq(new GTaskQ());
        new_p_taskq->Initialize();
        p_taskq = new_p_taskq;
    }
    pthread_mutex_lock(&gexecutor_svc_lock_);
    if (gexecutor_map_.find(executor_id) != gexecutor_map_.end()) {
        GExecutorSharedPtr executor = gexecutor_map_[executor_id];
        pthread_mutex_unlock(&gexecutor_svc_lock_);
        return executor;
    }
    GExecutorSharedPtr p_executor(
            new GAsyncExecutor(async_event_base,
                               p_taskq));
    p_executor->Initialize();
    gexecutor_map_[executor_id] = p_executor;
    pthread_mutex_unlock(&gexecutor_svc_lock_);
    return p_executor;
}

GExecutorSharedPtr GExecutorService::CreateSyncExecutor(
        const std::string& executor_id,
        size_t num_workers) {
    GTaskQSharedPtr p_taskq(new GTaskQ());
    GExecutorSharedPtr p_executor(new GSyncExecutor(p_taskq, num_workers));
    p_executor->Initialize();
    pthread_mutex_lock(&gexecutor_svc_lock_);
    gexecutor_map_[executor_id] = p_executor;
    pthread_mutex_unlock(&gexecutor_svc_lock_);
    return p_executor;
}
