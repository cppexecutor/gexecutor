/**
 * gsync_worker_thread.cc
 *  Copyright Admin Cppexecutor, 2013
 *  Created on: Jul 21, 2013
 *      Author: cppexecutor@gmail.com
 */

#include "gsync_worker_thread.h"

GSyncWorkerThread::GSyncWorkerThread(GSyncExecutor* sync_executor,
                                     GTaskQ* taskq,
                                     const std::string& worker_id)
    : sync_executor_(sync_executor), taskq_(taskq), thread_id_(),
      worker_id_(worker_id), event_base_(NULL), async_executor_(NULL)  {
}

GSyncWorkerThread::~GSyncWorkerThread() {
    if (async_executor_) {
        delete async_executor_;
        async_executor_ = NULL;
    }
    if (event_base_) {
        event_base_free(event_base_);
        event_base_ = NULL;
    }
}

static void *gsync_executor_worker(void *args) {

    GSyncWorkerThread* p_worker =
            static_cast<GSyncWorkerThread*>(args);
    GEXECUTOR_LOG(GEXECUTOR_TRACE)
            << "Starting Thread with threadid"
            << p_worker->id() << std::endl;
    p_worker->SetupEventLoop();
    event_base_dispatch(p_worker->event_base());
    return p_worker;
}

gerror_code_t GSyncWorkerThread::Initialize() {
    pthread_attr_t attr;
    int rc = 0;
    rc = pthread_attr_init(&attr);
    if (rc != 0) {
        GEXECUTOR_LOG(GEXECUTOR_ERROR)
                << "Error in pthread_attr_init:" << rc << std::endl;
    }
    rc = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    if (rc != 0) {
        GEXECUTOR_LOG(GEXECUTOR_ERROR)
                << "Error in pthread_attr_setdetachstate:" << rc << std::endl;
    }
    pthread_attr_setstacksize(&attr, 512*1024);
    /* Create one thread for each command-line argument */
    rc = pthread_create(&thread_id_, &attr,
                        &gsync_executor_worker,
                        this);
    GEXECUTOR_LOG(GEXECUTOR_TRACE)
        << "Created thread with id" << thread_id_ << std::endl;

    rc = pthread_attr_destroy(&attr);
    if (rc != 0) {
        GEXECUTOR_LOG(GEXECUTOR_ERROR)
                << "Error in pthread_attr_destroy:" << rc << std::endl;
    }
    return 0;
}

void GSyncWorkerThread::SetupEventLoop() {
    GEXECUTOR_LOG(GEXECUTOR_TRACE)
        << "Settting up Event loop for worker\n";
    event_base_ = event_base_new();
    async_executor_ = new GAsyncExecutor(event_base_,
                                         taskq_);
    async_executor_->Initialize();
}

void GSyncWorkerThread::Shutdown() {
    async_executor_->Shutdown();
}
