#GExecutor#


#Introduction#
GExector provides a unified way to handle synchronous and asynchronous tasks by abstracting tasks from underlying thread and process architecture. It is a C++ library which offers similar capabilities like Executor framework in Java and Twisted in Python.

One of the well studied patterns for implementing a High performance I/O bound applications is to use asynchronous processing of client requests. This allows for very high utilization of CPU, memory and saves context switches as different requests are served. Such a design pattern is also referred as reactor pattern reactor pattern using event loops like libevent or boost::asio. However, it has two limitations
1. **Synchronous Apis processing**: If application needs to call a synchronous API or processing then it would block the event loop. This would make the application perform badly as it breaks the reactor pattern. Typical strategies are to create threads and send offload tasks to it. However, there is no simple way to handle these tasks in C++.  
2. **Multi CPU limitation**: Typical event loops can only at most use a single CPU effectively. In today's computing environment has lots of CPU available and this could ultimately limit application performance. Programming languages like C++/C make it non trivial to use reactor patterns with multiple CPUs.

Multi-threaded or multi-process performance can outperform a single threaded systems by using concurrent data structures or shared memory in spite of added context switches and locking overheads by using a reactor pattern. However, such systems can have their own perils due to bad locking design (coarse) or deadlocks or additional complexity to use shared memory. More details on this discussion can be found at [C10K Problem](http://www.kegel.com/c10k.html).  

GExector offers a hybrid solution to take advantage of both paradigms and make it simple to use appropriate pattern for different aspects of application without loss of performance. It is a hybrid event loop based task processing framework that can be used for handling and routing tasks between async event loops and worker threads for handling synchronous tasks just like in a multi-threaded program.

GExecutor is inspired by Java Executor like [SEDA](http://www.eecs.harvard.edu/~mdw/proj/seda/) and [Twisted](http://twistedmatrix.com/trac/wiki) for python. It currently uses libevent based event loop for implementation on Linux.

#Architecture
GExector design is based on two important elements. Every executor has a task queue that it uses to receive Tasks. Each Task implements a virtual method *Execute()* that is called by the executor. Tasks contain pointer to the response Queue that can be used by the Task to send back a response to the originating Executor.

A DeferredTask template is a simple interface to use callback functions instead of creating a lower level task interface.

A DeferredTask supports three kinds of callbacks  
1. Task which is deferred onto a different execution context, an GExecutor. Eg. Worker thread.
2. Task Callback called with arguments as result of execution of the deferred task.
3. Task Errback called with captured error either during task execution or during the task callback execution.

#Example#
Here is an hello world example of a simple server based on GExecutor. Please see samples/simple_http_server for full reference:  

    /**
    * Hello world task that is executed in a different thread.
    */
    void print_hello() {
       std::cout << "Hello World\n";
    }
    /**
    * Callback called in the originator thread to indicate Hello World task was done and indeed it was successful in saying Hello!
    */
    void print_hello_done() {
       std::cout << "Said Hello to the world\n";
    }
    /**
    * Callback called if there was an exception in executing the Hello World task.
    */
    void print_hello_failed() {
        std::cout << "Could not say Hello\n";
    }  
    
    // Main thread that instantiates the GExecutor Service and sets up async and sync executors.
    
    int main(int argc, const char* argv[]) {
        // Creates a GExecutor reactor with default async executor loop
        GExecutorService executor_svc(true);
        // Creates a pool of synchronous workers that accepts tasks on taskq sync_executor_->taskq();
        sync_executor_ = executors_.CreateSyncExecutor("sync", num_sync_workers);
        // run the default reactor.
        executor_svc.run();
    }  
    
    // Example to create an async task for the default executor
    
    void create_async_task() {
        //Example to add print "Hello" Tasks
        GExecutorSharedPtr async_executor = executor_svc.gexecutor(
            executor_svc.kDefaultExecutorId);
        GTaskQSharedPtr taskq = async_executor->taskq();
        boost::shared_ptr<DeferredTask<void>> d(
            new DeferredTask<void>(taskq, print_hello);
        // attach callback when task print_hello was successful
        d.set_callback(print_hello_done);
        // attach callback when task print_hello failed.
        d.set_errback(print_hello_failed);
        async_executor->EnQueueTask(d);
    }

    // Example to create an sync task for the default executor

    void create_sync_task() {
        //Example to add print "Hello" Tasks
        GExecutorSharedPtr sync_executor = executor_svc.gexecutor(
            "sync");
        GTaskQSharedPtr taskq = sync_executor->taskq();
        boost::shared_ptr<DeferredTask<void>> d(
            new DeferredTask<void>(taskq, print_hello);
        // attach callback when task print_hello was successful
        d.set_callback(print_hello_done);
        // attach callback when task print_hello failed.
        d.set_errback(print_hello_failed);
        sync_executor->EnQueueTask(d);
    }


Here are some of the example reactor paradigms that can be easily implemented using GExecutor
##Single Async loop with synchronous worker pools##
![alt text](1async1sync.jpg)

There is one default async executor and one pool of synch workers. This is perhaps most common option for the I/O bound applications. CPU bound application would either need to break away tasks for workers or create multiple async execution blocks.

##Multiple Async loop using single synchronous worker pool
This is useful in the cases like HTTP server needs more than one CPU to just handle the TCP connection accepts. These accepts can be handled in a distributed executors. However, the request can then be sent to a common Executor/thread that is used for implementing the application using reactor design. This mechanism avoids need to use shared memory for sharing information between two threads/processes.
![alt text](multi-async-1-sync.jpg)


##Multiple Async loop with multiple synchronous worker pools##
This is useful in case of using event driven reactor approach to using multiple reactors to implement multi-stage SEDA style processing blocks. They can exchange tasks between them using the GEexecutor interface. In addition each stage can have its own synchronous processing as well. It can be used for having different worker queue lengths for different event processing units.
![alt text](multi-async-multi-sync.jpg)


#Design considerations#
One important aspect of the GExecutor interface is that it accepts taskq as a parameter for the async executors. This is because the taskq needs to be created before the Async threads are created. Otherwise the underlying FD and pipe interface used for notifications would not be visible outside the async thread created.


#Working with the Code#
##Requirements##
GExecutor is only supported on Linux. There is not much Linux specific implementation other than the fact that I have not had time to make it full platform dependent.

##Dependencies##
It is uses boost_system, libevent, GTest (unit tests), cmake (build). Script *setup.sh* can be used for simple installation.


##Installation##
Linux installation requires following steps:  
    git checkout https://github.com/cppexecutor/gexecutor.git
    mkdir build
    cd build
    cmake ../src
    make
    sudo make install
    make test
  
#Licensing#
All the software provided under gexecutor under the MIT License
