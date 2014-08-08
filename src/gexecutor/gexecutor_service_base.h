/**
 * gexecutor_server.h
 *  Copyright Admin Cppexecutor, 2013
 *  Created on: Jun 23, 2013
 *      Author: cppexecutor@gmail.com
 */

#ifndef GEXECUTOR_SERVICE_BASE_H_
#define GEXECUTOR_SERVICE_BASE_H_
#include "gexecutor/gexecutor.h"
#include <unordered_map>

/**
 * \brief A helper class to manage all executors (reactors)
 *
 *  This class provides a registry for all the executors. It also creates
 *  a default asynchronous reactor that owns the event_base.
 *
 *  Example.
    GExecutorServiceBase executor_svc(true);
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

class GExecutorServiceBase {
public:
    /**
     * ID of the default Async Executor
     */
    static const std::string kDefaultExecutorId;
    /**
     * @param enable_default_async_executor When set to true creates
     *  default async executor, event_base. This can be used for most
     *  applications which are starting new and don't already have an event
     *  base.
     *
     *  This should be set to False in case application already has an event
     *  base.
     */
    GExecutorServiceBase(bool enable_default_async_executor=false);
    virtual ~GExecutorServiceBase();
    /**
     *
     * @param gexecutor_id Calls shutdown of an executor and removes from the
     *  registery.
     * @return gerror_code_t success or error code if it fails.
     */
    gerror_code_t ShutdownExecutor(const std::string& gexecutor_id);

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
    virtual GExecutorSharedPtr CreateSyncExecutor(
            const std::string& executor_id,
            size_t num_default_threads) = 0;

    /**
     * @param gexecutor_id
     * @return shared pointer to the executor
     */
    GExecutorSharedPtr gexecutor(const std::string& gexecutor_id =
            kDefaultExecutorId);

    /**
     * Run the default asynchronous executor
     */
    virtual void run() = 0;

protected:
    /**
     * lock for guarding the executor map
     */
    pthread_mutex_t gexecutor_svc_lock_;
    /**
     * created only when service is instantiated with
     * enable_default_async_executor
     */
    GTaskQSharedPtr default_taskq_;
    GExecutorSharedPtr default_async_executor_;
    /**
     * map of all the executors.
     */
    std::unordered_map<std::string, GExecutorSharedPtr> gexecutor_map_;
    GEXECUTOR_DISALLOW_EVIL_CONSTRUCTORS(GExecutorServiceBase);
};

#endif /* GEXECUTOR_SERVICE_BASE_H_ */
