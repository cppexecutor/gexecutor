/**
 * gsync_executor.cc
 *  Copyright Admin Cppexecutor, 2013
 *  Created on: Jun 18, 2013
 *      Author: cppexecutor@gmail.com
 */

#include "gsync_executor_asio.h"
#include "gexecutor/asio/gsync_worker_thread_asio.h"
#include <set>


GSyncExecutorAsio::GSyncExecutorAsio(GTaskQSharedPtr taskq,
                             size_t num_workers)
    : GExecutor(GExecutorType::SYNC, taskq), num_workers_(num_workers) {
}

GSyncExecutorAsio::~GSyncExecutorAsio() {
    // TODO Auto-generated destructor stub
}

gerror_code_t GSyncExecutorAsio::EnQueueTask(GTaskSharedPtr task) {
    return p_taskq_->EnqueueGTask(task);
}

gerror_code_t GSyncExecutorAsio::Initialize() {
    if (num_workers_ == 0) {
        return 0;
    }

    for (size_t thindex = 0; thindex < num_workers_; thindex++) {
        std::string worker_id = std::to_string(thindex);
        GSyncWorkerThreadAsioSharedPtr p_worker(
                new GSyncWorkerThreadAsio(this, p_taskq_, worker_id));
        p_worker->Initialize();
        workers_.insert(p_worker);
    }
    return 0;
}

gerror_code_t GSyncExecutorAsio::Shutdown() {
    for (auto itr = workers_.begin(); itr != workers_.end(); itr++) {
        (*itr)->Shutdown();
        workers_.erase(itr);
        //delete (*itr);
    }
    return 0;
}
