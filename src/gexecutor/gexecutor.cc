/**
 * gexecutor.cc
 *  Copyright Admin Cppexecutor, 2013
 *  Created on: Jun 9, 2013
 *      Author: cppexecutor@gmail.com
 */

#include "gexecutor.h"

GExecutor::GExecutor(GExecutorType type,
                     //struct event_base *event_base,
                     GTaskQSharedPtr taskq)
    : //event_base_(event_base),
      gexec_type_(type), p_taskq_(taskq),
      num_enqueue_(0), num_dequeue_(0), num_task_with_response_(0),
      rate_of_dequeue_in_100secs(0), max_rate_of_dequeue(0),
      rate_of_enqueue_in_100secs(0), max_rate_of_enqueue(0) {
}

GExecutor::~GExecutor() {
    GEXECUTOR_LOG(GEXECUTOR_ERROR)
            << "GExecutor destructor called " << this << std::endl;
}


