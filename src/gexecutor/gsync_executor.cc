/**
 * gsync_executor.cc
 *  Copyright Admin Cppexecutor, 2013
 *  Created on: Jun 18, 2013
 *      Author: cppexecutor@gmail.com
 */

#include "gsync_executor.h"
#include "gsync_worker_thread.h"
#include <set>


GSyncExecutor::GSyncExecutor(GTaskQ* taskq,
                             size_t num_workers)
    : GExecutor(GExecutorType::SYNC, taskq), num_workers_(num_workers) {
}

GSyncExecutor::~GSyncExecutor() {
    // TODO Auto-generated destructor stub
}

gerror_code_t GSyncExecutor::EnQueueTask(GTask *task) {
    return 0;
}

gerror_code_t GSyncExecutor::Initialize() {
    if (num_workers_ == 0) {
        return 0;
    }

    for (size_t thindex = 0; thindex < num_workers_; thindex++) {
        std::string worker_id = std::to_string(thindex);
        GSyncWorkerThread *p_worker =
                new GSyncWorkerThread(this, p_taskq_, worker_id);
        p_worker->Initialize();
        workers_.insert(p_worker);
    }
    return 0;
}

gerror_code_t GSyncExecutor::Shutdown() {
    std::set<GSyncWorkerThread *>::iterator itr;
    for (itr = workers_.begin(); itr != workers_.end(); itr++) {
        (*itr)->Shutdown();
        delete (*itr);
    }
    return 0;
}
