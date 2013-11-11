/**
 * gexecutor_server.cc
 *  Copyright Admin Cppexecutor, 2013
 *  Created on: Jun 23, 2013
 *      Author: cppexecutor@gmail.com
 */

#include "gexecutor/gexecutor_service.h"
#include <pthread.h>
#include "gasync_executor.h"
#include "gsync_executor.h"


GExecutorService::GExecutorService() :
    gexecutor_lock_(PTHREAD_MUTEX_INITIALIZER), gexecutor_map_() {
}

GExecutorService::~GExecutorService() {
    pthread_mutex_lock(&gexecutor_lock_);
    for (auto itr = gexecutor_map_.begin(); itr != gexecutor_map_.end();
            itr++) {
        GExecutor *p_gexec = itr->second;
        p_gexec->Shutdown();
        gexecutor_map_.erase(itr);
    }
    pthread_mutex_unlock(&gexecutor_lock_);
    gexecutor_map_.clear();
}

void GExecutorService::Initialize() {
    pthread_mutex_init(&gexecutor_lock_, NULL);
}

gerror_code_t GExecutorService::DestroyExecutor(const std::string& gexecutor_id) {
    pthread_mutex_lock(&gexecutor_lock_);
    GExecutor *p_gexec = gexecutor_map_[gexecutor_id];
    if (p_gexec != NULL) {
        gexecutor_map_.erase(gexecutor_id);
    }
    pthread_mutex_unlock(&gexecutor_lock_);
    if (p_gexec == NULL) {
        return -1;
    }
    //p_gexec->Shutdown();
    if (p_gexec->type() == GExecutor::GExecutorType::SYNC) {
        delete p_gexec->taskq();
    }
    delete p_gexec;
    return 0;
}

GExecutor* GExecutorService::CreateAsyncExecutor(
        const std::string& executor_id, GTaskQ* p_taskq,
        struct event_base* async_event_base) {

    DestroyExecutor(executor_id);

    pthread_mutex_unlock(&gexecutor_lock_);
    if (p_taskq == NULL) {
        p_taskq = new GTaskQ();
    }
    GAsyncExecutor* p_executor =
            new GAsyncExecutor(async_event_base,
                               p_taskq);
    p_executor->Initialize();
    pthread_mutex_lock(&gexecutor_lock_);
    gexecutor_map_[executor_id] = p_executor;
    pthread_mutex_unlock(&gexecutor_lock_);
    return p_executor;
}

GExecutor* GExecutorService::CreateSyncExecutor(const std::string& executor_id,
                                                size_t num_workers) {
    DestroyExecutor(executor_id);
    GTaskQ* p_taskq = new GTaskQ();
    GSyncExecutor* p_executor = new GSyncExecutor(p_taskq, num_workers);
    p_executor->Initialize();
    pthread_mutex_lock(&gexecutor_lock_);
    gexecutor_map_[executor_id] = p_executor;
    pthread_mutex_unlock(&gexecutor_lock_);
    return p_executor;
}

GExecutor* GExecutorService::gexecutor(const std::string& gexecutor_id) {
    GExecutor* p_executor = NULL;
    pthread_mutex_lock(&gexecutor_lock_);
    p_executor = gexecutor_map_[gexecutor_id];
    pthread_mutex_unlock(&gexecutor_lock_);
    return p_executor;
}
