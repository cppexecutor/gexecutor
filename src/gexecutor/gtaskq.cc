/**
 * gtaskq.cc
 *  Copyright Admin Cppexecutor, 2013
 *  Created on: Jun 23, 2013
 *      Author: cppexecutor@gmail.com
 */

#include "gexecutor_common.h"
#include "gexecutor.h"
#include <queue>
#include <deque>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <assert.h>
#include "gtaskq.h"
#include <glog/logging.h>



GTask::GTask(GTaskQ* response_queue)
    : resp_task_q_(response_queue), exec_task_q_(NULL),
      executor_(NULL) {
}

GTask::~GTask() {
    return;
}

GTaskQ::GTaskQ()
    : num_enqueue_(0), num_dequeue_(0),
      max_outstanding_notifn_(4096), executor_ctx_(NULL),
      num_outstanding_notifn_(0),
      q_lock_(PTHREAD_MUTEX_INITIALIZER) {
    notfn_buffer_ = (char *) malloc(max_outstanding_notifn_);
    if (!notfn_buffer_) {
        return;
    }
    for (size_t indx = 0; indx < max_outstanding_notifn_; indx++) {
        notfn_buffer_[indx] = 'G';
    }
}

GTaskQ::~GTaskQ() {
    if (notfn_buffer_) {
        free(notfn_buffer_);
    }
    /**
     * close pipe
     */
    close(pipefds_[PIPE_FD_READ_INDX]);
    close(pipefds_[PIPE_FD_WRITE_INDX]);
}

gerror_code_t GTaskQ::Initialize() {
    pthread_mutex_init(&q_lock_, NULL);
    int rc = pipe2(pipefds_, O_NONBLOCK);
    if ( rc == -1) {
        perror("pipe");
        assert(0);
        return(errno);
    }
    return 0;
}

size_t GTaskQ::num_notifn_to_write(size_t num_new_notifn) {
    num_outstanding_notifn_+= num_new_notifn;
    size_t num_notifn_ = num_outstanding_notifn_;
    if (num_notifn_ > max_outstanding_notifn_) {
        num_notifn_ = max_outstanding_notifn_;
    }
    /**
     * substracting so that any other thread does not push in extra notifications
     *
     */
    num_outstanding_notifn_ -= num_notifn_;
    return num_notifn_;
}


gerror_code_t GTaskQ::NotifyQ(size_t num_notifn,
                              ssize_t *num_notified) {

    /**
     * If there are buffered notifications then send them in one shot.
     * Since Pipes have size limits so need to make sure it matches up with the
     * system in which this code is running.
     */
    int retry_count = 3;
    *num_notified = 0;
    while (retry_count && num_notifn) {
        GEXECUTOR_LOG(GEXECUTOR_TRACE)
            << "Num outstanding notification: "
            << num_outstanding_notifn_
            << " : num_new_notifn: " << num_notifn << std::endl;

        ssize_t rc = write(pipefds_[PIPE_FD_WRITE_INDX],
                           notfn_buffer_,
                           num_notifn);

        if (rc == -1) {
            GEXECUTOR_LOG(GEXECUTOR_ERROR)
                        << errno << ":" << strerror(errno) << std::endl;
        }

        if (retry_count) {
            retry_count--;
        }

        if ((rc == -1) && ( (errno == EAGAIN) || (errno == EINTR))) {
            continue;
        } else if (rc == -1) {
            assert(0);
            return errno;
        } else if (rc > 0) {
            *num_notified += rc;
            num_notifn -=rc;
            //num_outstanding_notifn_ -= rc;
            GEXECUTOR_LOG(GEXECUTOR_TRACE)
                << "Sent: " << rc << " notifications. num_outstanding: "
                << num_outstanding_notifn_
                << std::endl;
            continue;
        } else {
            GEXECUTOR_LOG(GEXECUTOR_TRACE)
                     << "no notification sent\n";
        }
    }
    return 0;
}

gerror_code_t GTaskQ::EnqueueGTask(GTask* task, GExecutor* executor_ctx) {
    /**
     * lock the queue and enqueue the task
     */
    ssize_t num_notified = 0;
    size_t num_notifn = 0;

    pthread_mutex_lock(&q_lock_);
    ++num_enqueue_;
    task_queue_.push_back(task);
    VLOG(GEXECUTOR_TRACE)
        << "Taskq Size: " << task_queue_.size() << std::endl;

    num_notifn = num_notifn_to_write(1);
    pthread_mutex_unlock(&q_lock_);

    NotifyQ(num_notifn, &num_notified);
    if ((num_notifn - num_notified) > 0) {
        // add outstanding back to the num_outstanding
        pthread_mutex_lock(&q_lock_);
        max_outstanding_notifn_ += (num_notifn - num_notified);
        pthread_mutex_unlock(&q_lock_);

    }
    return 0;
}

GTask* GTaskQ::DequeueGTask() {
    GTask* p_task = NULL;
    GEXECUTOR_LOG(GEXECUTOR_TRACE)
        << " taskq: " << this << " empty:" << task_queue_.empty()
        << " num: " << task_queue_.size()
        << " num dequeue: " << num_dequeue_
        << std::endl;
    pthread_mutex_lock(&q_lock_);
    if (!task_queue_.empty()) {
        p_task = task_queue_.front();
        task_queue_.pop_front();
        ++num_dequeue_;
    }
    pthread_mutex_unlock(&q_lock_);
    if (p_task) {
        GEXECUTOR_LOG(GEXECUTOR_TRACE)
            << p_task << std::endl;
        p_task->set_exec_task_q(this);
    } else {
        GEXECUTOR_LOG(GEXECUTOR_TRACE)
                        << "TaskQ Empty" << this << std::endl;
    }

    return p_task;
}
