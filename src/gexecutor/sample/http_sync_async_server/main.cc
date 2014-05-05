/**
 * main.cc
 *  Copyright Gaurav Rastogi, 2013
 *  Created on: May 4, 2014
 *      Author: grastogi
 */

#include "http_hybrid_server.h"
#include <iostream>
#include <glog/logging.h>
#include <gflags/gflags.h>
using namespace std;

int
main(int argc, char **argv)
{
    google::InitGoogleLogging(argv[0]);
    google::FlushLogFiles(google::GLOG_INFO);
    cout << "Starting Server" << std::endl;
    HTTPHybridServer httpd(FLAGS_num_sync_workers);
    cout << "Initializing\n";
    httpd.Initialize();
    cout << "Running server\n";
    httpd.Run();
    return 0;
}
