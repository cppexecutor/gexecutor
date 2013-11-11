/**
 * gexecutor_server.h
 *  Copyright Admin Cppexecutor, 2013
 *  Created on: Jun 23, 2013
 *      Author: cppexecutor@gmail.com
 */

#ifndef GEXECUTOR_SERVER_H_
#define GEXECUTOR_SERVER_H_
#include "gexecutor/gexecutor.h"
#include <unordered_map>

class GExecutorService {
public:
    GExecutorService();
    virtual ~GExecutorService();
    void Initialize();
    gerror_code_t DestroyExecutor(const std::string& gexecutor_id);
    GExecutor* CreateAsyncExecutor(
            const std::string& executor_id,
            GTaskQ *p_taskq,
            struct event_base *async_event_base);

    GExecutor* CreateSyncExecutor(
            const std::string& executor_id,
            size_t num_default_threads);
    GExecutor* gexecutor(const std::string& gexecutor_id);
private:
    pthread_mutex_t gexecutor_lock_;
    std::unordered_map<std::string, GExecutor*> gexecutor_map_;
};

#endif /* GEXECUTOR_SERVER_H_ */
