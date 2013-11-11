/**
 * gexecutor-common.h
 *  Copyright Admin Cppexecutor, 2013
 *  Created on: Jun 23, 2013
 *      Author: cppexecutor@gmail.com
 */

#ifndef GEXECUTOR_COMMON_H_
#define GEXECUTOR_COMMON_H_


#define GEXECUTOR_TRACE 0
#define GEXECUTOR_ERROR 1
#include <stdint.h>
#include <glog/logging.h>

typedef int64_t gerror_code_t;

#ifndef GEXECUTOR_LOG
#define GEXECUTOR_LOG(level) \
    VLOG(level)  \
        << "GEXECUTOR:[" << __FUNCTION__ << ":" << __LINE__ << "]"
#endif

#endif /* GEXECUTOR_COMMON_H_ */
