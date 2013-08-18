/**
 * gexecutor_server.h
 *  Copyright Admin Cppexecutor, 2013
 *  Created on: Jun 23, 2013
 *      Author: cppexecutor@gmail.com
 */

#ifndef GEXECUTOR_SERVER_H_
#define GEXECUTOR_SERVER_H_
#include "gexecutor/gexecutor.h"
#include <set>

class GExecutorServer {
public:
    GExecutorServer();
    virtual ~GExecutorServer();
    gerror_code_t AddExecutor(const std::string& name, GExecutor *p_executor);
    GExecutor *RemoveExecutor(const std::string& name);
    GExecutor *executor(GExecutorType type);
private:
    std::set<GExecutor *> executors_list_;
};

#endif /* GEXECUTOR_SERVER_H_ */
