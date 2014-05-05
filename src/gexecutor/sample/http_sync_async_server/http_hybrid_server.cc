/**
 * http_hybrid_server.cc
 *  Copyright Gaurav Rastogi, 2013
 *  Created on: May 4, 2014
 *      Author: grastogi
 */
#include "http_hybrid_server.h"
#include <event2/http.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>


DEFINE_int32(num_sync_workers, 2, "Number of threads for running tests");
DEFINE_int32(port, 4242, "Port number for HTTP server");
DEFINE_string(docroot, "/", "Default path for the web server");

static const struct table_entry {
    const char *extension;
    const char *content_type;
} content_type_table[] = {
    { "txt", "text/plain" },
    { "c", "text/plain" },
    { "h", "text/plain" },
    { "html", "text/html" },
    { "htm", "text/htm" },
    { "css", "text/css" },
    { "gif", "image/gif" },
    { "jpg", "image/jpeg" },
    { "jpeg", "image/jpeg" },
    { "png", "image/png" },
    { "pdf", "application/pdf" },
    { "ps", "application/postsript" },
    { NULL, NULL },
};

/* Try to guess a good content-type for 'path' */
static const char *
guess_content_type(const char *path)
{
    const char *last_period, *extension;
    const struct table_entry *ent;
    last_period = strrchr(path, '.');
    if (!last_period || strchr(last_period, '/'))
        goto not_found; /* no exension */
    extension = last_period + 1;
    for (ent = &content_type_table[0]; ent->extension; ++ent) {
        if (!evutil_ascii_strcasecmp(ent->extension, extension))
            return ent->content_type;
    }

not_found:
    return "application/misc";
}


HTTPHybridServer::HTTPHybridServer(uint32_t num_sync_workers) :
        docroot_(FLAGS_docroot.c_str()), num_sync_workers_(num_sync_workers),
        event_base_(0) {
}

HTTPHybridServer::~HTTPHybridServer() {
}

int32_t HTTPHybridServer::Run() {
    event_base_dispatch(event_base_);
    return 0;
}


void http_default_cb(struct evhttp_request * req, void *arg) {
    std::string *docroot = static_cast<std::string *>(arg);
    HTTPD_LOG(HTTPD_LOG_TRACE) << "DocRoot:" << *docroot << std::endl;
    const char *uri = evhttp_request_get_uri(req);
    const char *path;
    char *decoded_path;
    char whole_path[2048];
    int fd = -1;
    struct stat st;
    struct evhttp_uri *decoded = NULL;
    struct evbuffer *evb = NULL;

    if (evhttp_request_get_command(req) != EVHTTP_REQ_GET) {
        evhttp_send_error(req, HTTP_BADREQUEST, 0);
        return;
    }
    decoded = evhttp_uri_parse(uri);
    if (!decoded) {
        HTTPD_LOG(HTTPD_LOG_TRACE)
                << "It's not a good URI. Sending BADREQUEST\n";
        evhttp_send_error(req, HTTP_BADREQUEST, 0);
        return;
    }
    path = evhttp_uri_get_path(decoded);
    if (!path) path = "/";
    decoded_path = evhttp_uridecode(path, 0, NULL);
    if (decoded_path == NULL) {
        evhttp_send_error(req, HTTP_BADREQUEST, 0);
    }
    evutil_snprintf(whole_path, 2048, "%s/%s", docroot->c_str(), decoded_path);
    if (stat(whole_path, &st)<0) {
        evhttp_send_error(req, HTTP_BADREQUEST, 0);
    }
    evb = evbuffer_new();
    const char *type = guess_content_type(decoded_path);
    if ((fd = open(whole_path, O_RDONLY)) < 0) {

    }

    if (fstat(fd, &st) < 0) {
        /* Make sure the length still matches, now that we
         * opened the file :/ */
    }
    evhttp_add_header(evhttp_request_get_output_headers(req),
        "Content-Type", type);
    evbuffer_add_file(evb, fd, 0, st.st_size);

    evhttp_send_reply(req, 200, "OK", evb);
    if (decoded)
        evhttp_uri_free(decoded);
    if (decoded_path)
        free(decoded_path);
    if (evb)
        evbuffer_free(evb);
    return;
}

void http_async_request_cb(struct evhttp_request * req, void *arg) {
    std::string *docroot = static_cast<std::string *>(arg);
    HTTPD_LOG(HTTPD_LOG_TRACE) << "DocRoot:" << *docroot << std::endl;

}

void http_sync_request_cb(struct evhttp_request * req, void *arg) {
    std::string *docroot = static_cast<std::string *>(arg);
    HTTPD_LOG(HTTPD_LOG_TRACE) << "DocRoot:" << *docroot << std::endl;

}


int32_t HTTPHybridServer::Initialize() {
    event_base_ = event_base_new();
    if (!event_base_) {
        HTTPD_LOG(HTTPD_LOG_ERROR) << "Event Base null"
                <<  "errno: " << errno << ": " << strerror(errno) << std::endl;
        return errno;
    }
    HTTPD_LOG(HTTPD_LOG_TRACE) << "Created Event Base\n";

    evhttp_ = evhttp_new(event_base_);
    if (!evhttp_) {
        HTTPD_LOG(HTTPD_LOG_ERROR) << "EvHTTP creation failed. "
                <<  "errno: " << errno << ": " << strerror(errno) << std::endl;
        return errno;
    }
    HTTPD_LOG(HTTPD_LOG_TRACE) << "Created HTTP Event \n";

    evhttp_set_gencb(evhttp_, http_default_cb, &docroot_);

    evhttp_set_cb(evhttp_, "/sync", http_sync_request_cb, &docroot_);

    evhttp_set_cb(evhttp_, "/async", http_async_request_cb, &docroot_);

    HTTPD_LOG(HTTPD_LOG_TRACE) << "Setup callbacks\n";

    HTTPD_LOG(HTTPD_LOG_TRACE) << "Creating http socket handle\n";

    /* Now we tell the evhttp what port to listen on */
    socket_handle_ = evhttp_bind_socket_with_handle(evhttp_, "0.0.0.0",
                                                    FLAGS_port);
    if (!socket_handle_) {
        HTTPD_LOG(HTTPD_LOG_ERROR) << "couldn't bind to port"
                << FLAGS_port
                <<  "errno: " << errno << ": " << strerror(errno) << std::endl;
        return errno;
    }

    HTTPD_LOG(HTTPD_LOG_TRACE) << "Created http listener\n";

    return 0;
}
