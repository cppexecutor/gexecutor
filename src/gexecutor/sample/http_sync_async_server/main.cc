/**
 * main.cc
 *  Copyright Gaurav Rastogi, 2013
 *  Created on: May 4, 2014
 *      Author: grastogi
 */

#include <gtest/gtest.h>
#include "http_hybrid_server.h"
#include <iostream>
#include <glog/logging.h>
#include <gflags/gflags.h>

using namespace std;


int
main(int argc, char **argv)
{
    HTTPHybridServer httpd(FLAGS_num_sync_workers);

    httpd.Initialize();
    httpd.Run();

//    struct event_base *base;
//    struct evhttp *http;
//    struct evhttp_bound_socket *handle;
//
//    unsigned short port = 0;
//
//    if (argc < 2) {
//        syntax();
//        return 1;
//    }
//
//    base = event_base_new();
//    if (!base) {
//        fprintf(stderr, "Couldn't create an event_base: exiting\n");
//        return 1;
//    }
//
//    /* Create a new evhttp object to handle requests. */
//    http = evhttp_new(base);
//    if (!http) {
//        fprintf(stderr, "couldn't create evhttp. Exiting.\n");
//        return 1;
//    }
//
//    /* The /dump URI will dump all requests to stdout and say 200 ok. */
//    evhttp_set_cb(http, "/dump", dump_request_cb, NULL);
//
//    /* We want to accept arbitrary requests, so we need to set a "generic"
//     * cb.  We can also add callbacks for specific paths. */
//    evhttp_set_gencb(http, send_document_cb, argv[1]);
//
//    /* Now we tell the evhttp what port to listen on */
//    handle = evhttp_bind_socket_with_handle(http, "0.0.0.0", port);
//    if (!handle) {
//        fprintf(stderr, "couldn't bind to port %d. Exiting.\n",
//            (int)port);
//        return 1;
//    }
//
//    {
//        /* Extract and display the address we're listening on. */
//        struct sockaddr_storage ss;
//        evutil_socket_t fd;
//        ev_socklen_t socklen = sizeof(ss);
//        char addrbuf[128];
//        void *inaddr;
//        const char *addr;
//        int got_port = -1;
//        fd = evhttp_bound_socket_get_fd(handle);
//        memset(&ss, 0, sizeof(ss));
//        if (getsockname(fd, (struct sockaddr *)&ss, &socklen)) {
//            perror("getsockname() failed");
//            return 1;
//        }
//        if (ss.ss_family == AF_INET) {
//            got_port = ntohs(((struct sockaddr_in*)&ss)->sin_port);
//            inaddr = &((struct sockaddr_in*)&ss)->sin_addr;
//        } else if (ss.ss_family == AF_INET6) {
//            got_port = ntohs(((struct sockaddr_in6*)&ss)->sin6_port);
//            inaddr = &((struct sockaddr_in6*)&ss)->sin6_addr;
//        } else {
//            fprintf(stderr, "Weird address family %d\n",
//                ss.ss_family);
//            return 1;
//        }
//        addr = evutil_inet_ntop(ss.ss_family, inaddr, addrbuf,
//            sizeof(addrbuf));
//        if (addr) {
//            printf("Listening on %s:%d\n", addr, got_port);
//            evutil_snprintf(uri_root, sizeof(uri_root),
//                "http://%s:%d",addr,got_port);
//        } else {
//            fprintf(stderr, "evutil_inet_ntop failed\n");
//            return 1;
//        }
//    }
//
//    event_base_dispatch(base);

    return 0;
}
