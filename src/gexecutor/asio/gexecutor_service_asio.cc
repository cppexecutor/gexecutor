/**
 * gexecutor_server.cc
 *  Copyright Admin Cppexecutor, 2013
 *  Created on: Jun 23, 2013
 *      Author: cppexecutor@gmail.com
 */

#include <pthread.h>
#include "gexecutor/asio/gexecutor_service_asio.h"
#include "gexecutor/asio/gasync_executor_asio.h"
#include "gexecutor/asio/gsync_executor_asio.h"


GExecutorServiceAsio::GExecutorServiceAsio(bool enable_default_async_executor) :
    GExecutorServiceBase(enable_default_async_executor),
    default_io_service_() {

    if (enable_default_async_executor == false) {
        return;
    }
    default_async_executor_ =
            CreateAsyncExecutor(kDefaultExecutorId, default_taskq_,
                                default_io_service_);
}

GExecutorServiceAsio::~GExecutorServiceAsio() {
}

GExecutorSharedPtr GExecutorServiceAsio::CreateAsyncExecutor(
        const std::string& executor_id,
        GTaskQSharedPtr p_taskq,
        boost::asio::io_service& io_service) {
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

    // TODO - fix code
    GExecutorSharedPtr p_executor(
            new GAsyncExecutorAsio(io_service,
                                   p_taskq));
    p_executor->Initialize();
    gexecutor_map_[executor_id] = p_executor;
    pthread_mutex_unlock(&gexecutor_svc_lock_);
    return p_executor;
}

GExecutorSharedPtr GExecutorServiceAsio::CreateSyncExecutor(
        const std::string& executor_id,
        size_t num_workers) {
    GTaskQSharedPtr p_taskq(new GTaskQ());
    GExecutorSharedPtr p_executor(new GAsyncExecutorAsio(default_io_service_,
                                                         p_taskq));
    p_executor->Initialize();
    pthread_mutex_lock(&gexecutor_svc_lock_);
    gexecutor_map_[executor_id] = p_executor;
    pthread_mutex_unlock(&gexecutor_svc_lock_);
    return p_executor;
}
