/**
 * http_hybrid_server.cc
 *  Copyright Gaurav Rastogi, 2013
 *  Created on: May 4, 2014
 *      Author: grastogi
 */
#include "http_hybrid_server.h"

DEFINE_int32(num_sync_workers, 2, "Number of threads for running tests");
DEFINE_int32(port, 8080, "Port number for HTTP server");
DEFINE_string(path, "/", "Default path for the web server");

HTTPHybridServer::HTTPHybridServer(uint32_t num_sync_workers):
    num_sync_workers_(num_sync_workers), event_base_(0) {
    // TODO Auto-generated constructor stub

}

HTTPHybridServer::~HTTPHybridServer() {
    // TODO Auto-generated destructor stub
}

void HTTPHybridServer::Run() {

}

void HTTPHybridServer::Initialize() {


}
