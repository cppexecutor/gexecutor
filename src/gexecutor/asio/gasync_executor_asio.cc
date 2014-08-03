/**
 * gasync_executor.cpp
 *  Copyright Admin Cppexecutor, 2013
 *  Created on: Jun 9, 2013
 *      Author: cppexecutor@gmail.com
 */

#include "gasync_executor_asio.h"
#include <assert.h>
#include <iostream>
#include <glog/logging.h>
#include <asio/io_service.hpp>
#include <asio/posix/basic_stream_descriptor.hpp>


GAsyncExecutorAsio::GAsyncExecutorAsio(boost::asio::io_service& io_service,
                                       GTaskQSharedPtr taskq)
    : GExecutor(GExecutorType::ASYNC, taskq),
      io_service_(io_service), taskq_read_desc_(io_service),
      taskq_write_desc_(io_service),
      taskq_msgs_(),
      check_taskq_timer_(io_service){
    /**
     *  When a async executor is created then following is done
     *  1. setup a queue for receiving events
     *  2. setup an event for these queues
     *  3. Ensure there are no thundering herd issues.
     */
}

GAsyncExecutorAsio::~GAsyncExecutorAsio() {
}

gerror_code_t GAsyncExecutorAsio::Shutdown() {
    StopTimer();
    taskq_read_desc_.close();
    taskq_write_desc_.close();
    return 0;
}

void GAsyncExecutorAsio::StopTimer() {
}

gerror_code_t GAsyncExecutorAsio::EnQueueTask(GTaskSharedPtr task) {
    return p_taskq_->EnqueueGTask(task);
}

//static void taskq_cb(evutil_socket_t fd, short what, void *arg) {
//    char msg[4096];
//    ssize_t num_bytes = 0;
//    GExecutor* executor = static_cast<GExecutor *>(arg);
//    GTaskQSharedPtr p_taskq = executor->taskq();
//    snprintf(msg, 128, "Got an event on socket %d:%s%s%s%s Taskq[%p]",
//             (int) fd,
//             (what&EV_TIMEOUT) ? " timeout" : "",
//             (what&EV_READ)    ? " read" : "",
//             (what&EV_WRITE)   ? " write" : "",
//             (what&EV_SIGNAL)  ? " signal" : "",
//             arg);
//
//    GEXECUTOR_LOG(GEXECUTOR_TRACE) << msg << std::endl;
//    if (!(what & EV_READ)) {
//        return;
//    }
//
//    do {
//        num_bytes = read(fd, msg, 4096);
//        if (num_bytes == -1) {
//            GEXECUTOR_LOG(GEXECUTOR_TRACE)
//                    << "Read: "<< num_bytes << " from fd: " << fd
//                    << " errno: " << errno
//                    << "strerr: " << strerror(errno) << std::endl;
//            return;
//        }
//        //GEXECUTOR_LOG(GEXECUTOR_TRACE)
//        //           << "Read: "<< num_bytes << " from fd: " << fd << std::endl;
//        for (int num_tasks = 0; num_tasks < num_bytes; num_tasks++) {
//            GTaskSharedPtr p_task = p_taskq->DequeueGTask();
//            GEXECUTOR_LOG(GEXECUTOR_TRACE)
//                << "Executing task " << p_task->DebugString() << std::endl;
//            if (p_task) {
//                p_task->set_executor(executor);
//                p_task->Execute();
//            }
//        }
//    } while (num_bytes > 0);
//    return;
//}


class GTaskCheckTaskQ : public GTask {
public:
    GTaskCheckTaskQ(GTaskQSharedPtr taskq)
    : GTask(taskq) {
    }
    virtual ~GTaskCheckTaskQ() {
        return;
    }
protected:
    virtual gerror_code_t Execute() {
        //VLOG(GEXECUTOR_TRACE) << "Task Q working fine" << std::endl;
        return 0;
    }
};


//static void check_taskq_cb(evutil_socket_t fd, short what, void *arg) {
//    GAsyncExecutorAsio *executor = static_cast<GAsyncExecutorAsio *>(arg);
//    VLOG(GEXECUTOR_TRACE) << __FUNCTION__ << ": checking task callback"
//            << executor->taskq() << arg << std::endl;
//    GTaskSharedPtr check_task(new GTaskCheckTaskQ(executor->taskq()));
//    executor->taskq()->EnqueueGTask(check_task);
//}


void GAsyncExecutorAsio::taskq_read_handler(const error_code& ec,
                        std::size_t bytes_transferred) {
    if (ec) {
        GEXECUTOR_LOG(GEXECUTOR_ERROR)
                   << "Read Error"<< ec << std::endl;
        return;
    }
    std::size_t num_bytes = bytes_transferred;
    if (num_bytes == 0) {
        num_bytes = 1;
    }
    GTaskQSharedPtr p_taskq = taskq();
    //GEXECUTOR_LOG(GEXECUTOR_TRACE)
    //           << "Read: "<< num_bytes << " from fd: " << fd << std::endl;
    for (std::size_t num_tasks = 0; num_tasks < num_bytes; num_tasks++) {
        GTaskSharedPtr p_task = p_taskq->DequeueGTask();
        if (p_task) {
            GEXECUTOR_LOG(GEXECUTOR_TRACE)
                << "Executing task " << p_task->DebugString() << std::endl;
            p_task->set_executor(this);
            p_task->Execute();
        }
    }
    return;

}


gerror_code_t GAsyncExecutorAsio::Initialize() {
    p_taskq_->Initialize();
    //executor_shared_ptr_ = shared_from_this();
    /**
     * Add event for reading from the queue
     */


    taskq_read_desc_.assign(p_taskq_->read_fd());
    taskq_write_desc_.assign(p_taskq_->write_fd());

    taskq_read_desc_.async_read_some(
            boost::asio::buffer(taskq_msgs_),
            boost::bind(&GAsyncExecutorAsio::taskq_read_handler, this,
                        boost::asio::placeholders::error,
                        boost::asio::placeholders::bytes_transferred));

    VLOG(GEXECUTOR_TRACE) << "Setup Timer event for async engine "
            << taskq() << "\n";

    return 0;
}


