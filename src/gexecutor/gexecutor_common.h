/**
 * gexecutor-common.h
 *  Copyright Admin Cppexecutor, 2013
 *  Created on: Jun 23, 2013
 *      Author: cppexecutor@gmail.com
 */

#ifndef GEXECUTOR_COMMON_H_
#define GEXECUTOR_COMMON_H_



#include <stdint.h>
#include <glog/logging.h>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

typedef int64_t gerror_code_t;
#define GEXECUTOR_TRACE 0
#define GEXECUTOR_ERROR 1

#ifndef GEXECUTOR_LOG
#define GEXECUTOR_LOG(level) \
    VLOG(level)  \
        << "GEXECUTOR:[" << __FUNCTION__ << ":" << __LINE__ << "]"
#endif

#ifndef GEXECUTOR_DISALLOW_EVIL_CONSTRUCTORS
#define GEXECUTOR_DISALLOW_EVIL_CONSTRUCTORS(TypeName)    \
  TypeName(const TypeName&);                           \
  void operator=(const TypeName&)
#endif

class GTask;
class GExecutor;
class GTaskQ;

typedef boost::shared_ptr<GTaskQ> GTaskQSharedPtr;
typedef boost::shared_ptr<GExecutor> GExecutorSharedPtr;
typedef boost::shared_ptr<GTask> GTaskSharedPtr;

#endif /* GEXECUTOR_COMMON_H_ */
