/**
 * main.cc
 *  Copyright Gaurav Rastogi, 2013
 *  Created on: May 4, 2014
 *      Author: grastogi
 */

#include "simple_http_server.h"
#include <iostream>
#include <glog/logging.h>
#include <gflags/gflags.h>
using namespace std;

int
main(int argc, char **argv)
{
    google::InitGoogleLogging(argv[0]);
    FLAGS_logtostderr = 0;
    google::ParseCommandLineFlags(&argc, &argv, true);

    google::FlushLogFiles(google::GLOG_INFO);

    cout << "Starting Server" << std::endl;
    SimpleHttpServer httpd(FLAGS_num_sync_workers);
    cout << "Initializing\n";
    httpd.Initialize();
    cout << "Running server\n";
    httpd.Run();
    return 0;
}
