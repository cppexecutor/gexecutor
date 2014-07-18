/**
 * gexecutor_server.cc
 *  Copyright Admin Cppexecutor, 2013
 *  Created on: Jun 23, 2013
 *      Author: cppexecutor@gmail.com
 */

#include "gexecutor/gexecutor_service_base.h"
#include <pthread.h>
#include "gasync_executor.h"
#include "gsync_executor.h"

const std::string GExecutorServiceBase::kDefaultExecutorId = "DefaultExecutor";

GExecutorServiceBase::GExecutorServiceBase(bool enable_default_async_executor) :
    gexecutor_svc_lock_(PTHREAD_MUTEX_INITIALIZER),
    default_taskq_(NULL),
    default_async_executor_(),
    gexecutor_map_() {
    pthread_mutex_init(&gexecutor_svc_lock_, NULL);

    if (enable_default_async_executor == false) {
        return;
    }
    GTaskQSharedPtr taskq(new GTaskQ());
    default_taskq_ = taskq;
    default_taskq_->Initialize();
}

GExecutorServiceBase::~GExecutorServiceBase() {
    pthread_mutex_lock(&gexecutor_svc_lock_);
    for (auto itr = gexecutor_map_.begin(); itr != gexecutor_map_.end();
            itr++) {
        GExecutorSharedPtr p_gexec = itr->second;
        p_gexec->Shutdown();
        gexecutor_map_.erase(itr);
    }
    pthread_mutex_unlock(&gexecutor_svc_lock_);
    gexecutor_map_.clear();
}

gerror_code_t GExecutorServiceBase::ShutdownExecutor(const std::string& gexecutor_id) {
    pthread_mutex_lock(&gexecutor_svc_lock_);
    GExecutorSharedPtr p_gexec = gexecutor_map_[gexecutor_id];
    if (p_gexec != NULL) {
        gexecutor_map_.erase(gexecutor_id);
    }
    pthread_mutex_unlock(&gexecutor_svc_lock_);
    if (p_gexec == NULL) {
        return -1;
    }
    p_gexec->Shutdown();
    return 0;
}

GExecutorSharedPtr GExecutorServiceBase::gexecutor(
        const std::string& gexecutor_id) {
    GExecutorSharedPtr p_executor;
    pthread_mutex_lock(&gexecutor_svc_lock_);
    if (gexecutor_map_.find(gexecutor_id) != gexecutor_map_.end()) {
        p_executor = gexecutor_map_[gexecutor_id];
    }
    pthread_mutex_unlock(&gexecutor_svc_lock_);
    return p_executor;
}
