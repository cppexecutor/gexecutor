/**
 * gexecutor_unittest.h
 *  Copyright Gaurav Rastogi, 2013
 *  Created on: Aug 24, 2013
 *      Author: grastogi
 */

#ifndef GEXECUTOR_UNITTEST_H_
#define GEXECUTOR_UNITTEST_H_
#include <gexecutor_service.h>
#include <gasync_executor.h>
#include <gsync_executor.h>


class GExecutorTest : public testing::Test {
public:
    void CheckForEventExecution(std::set<bool *> events_list) {
        return;
    }

protected:
    struct event_base *event_base_;
    GExecutorService g_svc_;
    virtual void SetUp() {
        event_base_ = event_base_new();
        if (!event_base_) {
            ASSERT_TRUE(event_base_ != NULL);
            GEXECUTOR_LOG(GEXECUTOR_TRACE) << "Event Base null\n";
            return;
        }
        g_svc_.Initialize();

    }
    virtual void TearDown() {
        if (event_base_) {
            free(event_base_);
        }
    }
private:
};




class GTaskHello : public GTask {
public:
    GTaskHello(GTaskQSharedPtr taskq)
    : GTask(taskq), id_(0) {
    }
    virtual ~GTaskHello() {
        return;
    }
    void set_id(int id) {
           id_ = id;
    }
protected:
    virtual gerror_code_t Execute() {
        GEXECUTOR_LOG(GEXECUTOR_TRACE) << "Hello Task ID" << id_ << std::endl;
        //delete this;
        return 0;
    }
private:
    int id_;
};


class GTaskPing : public GTask {
public:
    GTaskPing(GTaskQSharedPtr resp_task_q, const std::string& msg)
        : GTask(resp_task_q), id_(0), msg_(msg) {
    }
    virtual ~GTaskPing() {
        return;
    }
    void set_id(int id) {
           id_ = id;
    }
    const std::string& DebugString() {
        return msg_;
    }
protected:
    gerror_code_t Execute();
private:
    int id_;
    std::string msg_;
};

class GTaskCheckTaskQ : public GTask {
public:
    GTaskCheckTaskQ(GTaskQSharedPtr taskq)
    : GTask(taskq), id_(0) {
    }
    virtual ~GTaskCheckTaskQ() {
        return;
    }
    void set_id(int id) {
        id_ = id;
    }
protected:
    virtual gerror_code_t Execute() {
        GEXECUTOR_LOG(GEXECUTOR_TRACE)
                << "Hello Task SRC ID" << id_ << std::endl;
        //delete this;
        return 0;
    }
private:
    int id_;
};


class ThreadInfo {
public:
    enum GExecutorTestType {
        HELLO=0,
        PINGPONG=1,
        HELLO_LOOP=2,
    };

    pthread_t thread_id;        /* ID returned by pthread_create() */
    int       thread_num;       /* Application-defined thread # */
    int       max_events;
    GTaskQSharedPtr   taskq;
    static    int num_queues;
    struct event *timer_ev;
    struct event_base* async_base;
    void (*start_routine)(evutil_socket_t fd, short what, void *arg);
    GExecutorTestType task_type;
    static std::vector<GExecutor *>& executor_list();
    bool is_thread_stopped;
    bool ping_started;
    GExecutorService* p_svc;
private:
    static    std::vector<GExecutor*>* executor_list_;
};


class GSyncExecutorTest : public testing::Test {
public:

protected:
    struct event_base *event_base_;
    virtual void SetUp() {
        event_base_ = event_base_new();
        if (!event_base_) {
            ASSERT_TRUE(event_base_ != NULL);
            GEXECUTOR_LOG(GEXECUTOR_TRACE) << "Event Base null\n";
            return;
        }

    }
    virtual void TearDown() {
        if (event_base_) {
            event_base_free(event_base_);
        }
    }
};


class HybridApp {
public:
    GExecutor* sync;
    GExecutor* async;
    struct event *timer_ev;
    struct event_base *async_base;
};


#endif /* GEXECUTOR_UNITTEST_H_ */
