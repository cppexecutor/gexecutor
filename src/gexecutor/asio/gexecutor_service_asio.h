/**
 * gexecutor_server.h
 *  Copyright Admin Cppexecutor, 2013
 *  Created on: Jun 23, 2013
 *      Author: cppexecutor@gmail.com
 */

#ifndef GEXECUTOR_SERVICE_ASIO_H_
#define GEXECUTOR_SERVICE_ASIO_H_
#include <event2/event.h>
#include "gexecutor/gexecutor_service_base.h"
#include "gexecutor/gexecutor.h"
#include <unordered_map>

/**
 * \brief A helper class to manage all executors based on libevent (reactors)
 *
 *  This class provides a registry for all the executors. It also creates
 *  a default asynchronous reactor that owns the event_base.
 *
 *  Example.
    GExecutorServiceAsio executor_svc(true);
    // get access to the default event_base
    struct event_base *event_base = executor_svc.event_base();
 *  // run the default reactor.
    executor_svc.run();
 *
    void print_hello() {
       std::cout << "Hello World\n";
    }
 *
    void print_hello_done() {
       std::cout << "Said Hello to the world\n";
    }
 *
 *  Example to add print "Hello" Tasks
    GExecutorSharedPtr async_executor = executor_svc.gexecutor(
        executor_svc.kDefaultExecutorId);
    GTaskQSharedPtr taskq = async_executor->taskq();
    boost::shared_ptr<DeferredTask<void>> d(
            new DeferredTask<void>(taskq, print_hello);
    d.set_callback(print_hello_done);
    async_executor->EnQueueTask(d);
 *
 */

class GExecutorServiceAsio : public GExecutorServiceBase {
  public:
    /**
     * @param enable_default_async_executor When set to true creates
     *  default async executor, event_base. This can be used for most
     *  applications which are starting new and don't already have an event
     *  base.
     *
     *  This should be set to False in case application already has an event
     *  base.
     */
    GExecutorServiceAsio(bool enable_default_async_executor=false);
    virtual ~GExecutorServiceAsio();
    /**
     * Creates a new Asynchronous Executor
     * @param executor_id Executor ID that can be used for referencing executor
     * @param p_taskq External TaskQ for the executor. TaskQ is taken as an
     *      input as TaskQ needs to be created before the thread running
     *      the executor is created. When multiple asynchronous executors
     *      are present then taskq to communicate between them needs to
     *      be created before the thread running executor is created.
     *
     * @param async_event_base. Event Base to be used for the executor.
     * @return shared pointer to the executor.
     */
    GExecutorSharedPtr CreateAsyncExecutor(
            const std::string& executor_id,
            GTaskQSharedPtr p_taskq,
            struct event_base *async_event_base);

    /**
     * Creates a new Synchronous Executor with number of threads.
     *
     * Typically there is no need to have multiple thread pools. However, some
     * applications may have such a requirement where each processing engine
     * needs its own worker pool.
     *
     * @param executor_id Executor ID that can be used for referencing executor
     * @param num_default_threads Number of workers in worker pool
     * @return shared pointer to the executor.
     */
    GExecutorSharedPtr CreateSyncExecutor(
            const std::string& executor_id,
            size_t num_default_threads);

    /**
     * Run the default asynchronous executor
     */
    void run() {
        event_base_dispatch(default_async_event_base_);
    }

    /**
     * @return default event_base
     */
    struct event_base * event_base() {
        return default_async_event_base_;
    }

private:
    /**
     * created only when service is instantiated with
     * enable_default_async_executor
     */
    struct event_base *default_async_event_base_;
    GEXECUTOR_DISALLOW_EVIL_CONSTRUCTORS(GExecutorServiceAsio);
};

#endif /* GEXECUTOR_SERVICE_ASIO_H_ */
