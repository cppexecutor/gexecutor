/**
 * gasyn_cexecutor.h
 *  Copyright Admin Cppexecutor, 2013
 *  Created on: Jun 9, 2013
 *      Author: cppexecutor@gmail.com
 */

#ifndef GASYN_EXECUTOR_ASIO_H_
#define GASYN_EXECUTOR_ASIO_H_

#include <asio/io_service.hpp>
#include <boost/asio/posix/stream_descriptor.hpp>
#include <boost/system/error_code.hpp>
#include <boost/array.hpp>
#include <boost/asio/placeholders.hpp>
#include <boost/bind.hpp>
#include "gexecutor/gexecutor.h"
#include <boost/asio/steady_timer.hpp>

using boost::asio::posix::stream_descriptor;
using boost::system::error_code;

/**
 *  \description
 *  This implements a asynchronous executor engine where tasks are expected to
 *  be processed in asynchronous manner.
 *
 *  Following example shows simple steps to create a Synchronous Executor
 *  with worker threads.
 *  // create taskq where all the synchronous workers woudl listen
 *  GTaskQSharedPtr async_taskq(new GTaskQ());
 *  async_taskq->Initialize();
 *  GAsyncExecutorAsio *async_engine =
 *           new GAsyncExecutorAsio(async_taskq);
 *  // Now sync engine is ready for events
 *  Send tasks to the sync engine
 *
 *  boost::shared_ptr<DeferredTask<void>> d(
 *          new DeferredTask<void>(taskq, print_hello);
 *  // attach callback when task print_hello was successful
 *  d.set_callback(print_hello_done);
 *  // attach callback when task print_hello failed.
 *  d.set_errback(print_hello_failed);
 *  async_executor->EnQueueTask(d);
 */
class GAsyncExecutorAsio : public GExecutor {
public:
    GAsyncExecutorAsio(boost::asio::io_service& io_service,
                   GTaskQSharedPtr taskq);
    virtual ~GAsyncExecutorAsio();
    virtual gerror_code_t Initialize();
    virtual gerror_code_t EnQueueTask(GTaskSharedPtr task);
//    virtual gerror_code_t EnQueueTask(GTask *task,
//                                      GExecutor* task_originator_ctx);
    /**
     *  This is used
     */
    virtual GTaskQSharedPtr taskq() {
        return p_taskq_;
    }
    virtual gerror_code_t Shutdown();

    virtual void StopTimer();

protected:
    boost::asio::io_service& io_service_;
    stream_descriptor taskq_read_desc_;
    stream_descriptor taskq_write_desc_;
    boost::array<char, 8192> taskq_msgs_;
    boost::asio::steady_timer check_taskq_timer_;
private:
    void taskq_read_handler(const error_code& ec,
                            std::size_t bytes_transferred);
    GEXECUTOR_DISALLOW_EVIL_CONSTRUCTORS(GAsyncExecutorAsio);
};
typedef boost::shared_ptr<GAsyncExecutorAsio> GAsyncExecutorAsioSharedPtr;


#endif /* GASYN_EXECUTOR_ASIO_H_ */
