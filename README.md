GExecutor
=========

Introduction
============
GExector provides a C++ library provides a unified way to handle synchronous and asynchronous tasks by abstracting the tasks from underlying thread and process architecture. 

It is inspired by Java Executor like [SEDA](http://www.eecs.harvard.edu/~mdw/proj/seda/) and [Twisted](http://twistedmatrix.com/trac/wiki) for python.

In a typical I/O bound application can be implemented via reactor pattern using event loops like libevent or boost::asio. However, it has two limitations
1. Synchronous Apis processing: If application needs to call a synchronous API or processing then it would block the event loop. This would make the application perform badly as it breaks the reactor pattern. Typical strategies are to create threads and send offload tasks to it. However, there is no simple way to handle these tasks in C++.
2. Single CPU limitation: Typical event loops can only at most use a single CPU effectively. In today's computing environment has lots of CPU available. However, without using concurrent data structures it is not trivial to use reactor pattern and also use multiple CPU without complicating the programming paradigm.

An alternative stragegy is to use multiple threads or process to use better utilize multiple CPUs. However, that can complicate the application design once locks or shared memory based information sharing is used between the different computing blocks. More details on the discussion can be found at [C10K Problem](http://www.kegel.com/c10k.html).

GExector implements a hybrid event loop based task processing framework that can be used for handling and routing tasks between async event loops and worker threads for handling synchronous tasks.


=======


It currently uses libevent based event loop for implementation on Linux.

Working with the Code
=====================

Requirements
------------
GExecutor is only supported on Linux. There is not much Linux specific implementation other than the fact that I have not had time to make it full platform dependent.

Dependencies
------------
It is uses boost_system, libevent, GTest (unit tests).

Checkout code
-------------

Installation
------------


Licensing
=========
All the software provided under gexecutor under the MIT License


