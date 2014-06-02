/**
 * http_hybrid_server.cc
 *  Copyright Gaurav Rastogi, 2013
 *  Created on: May 4, 2014
 *      Author: grastogi
 */
#include "simple_http_server.h"
#include <event2/http.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <memory>

DEFINE_int32(num_sync_workers, 2, "Number of threads for running tests");
DEFINE_int32(port, 4242, "Port number for HTTP server");
DEFINE_string(docroot, "/var/www/html", "Default path for the web server");

using namespace std;

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


SimpleHttpServer::~SimpleHttpServer() {
}

int32_t SimpleHttpServer::Run() {
    event_base_dispatch(event_base_);
    return 0;
}


void http_req_cleanup(char *decoded_path,
                      struct evhttp_uri *decoded,
                      struct evbuffer *evb) {
    if (decoded)
        evhttp_uri_free(decoded);
    if (decoded_path)
        free(decoded_path);
    if (evb)
        evbuffer_free(evb);
    return;
}



void http_async_request_cb(struct evhttp_request * req, void *arg) {
    SimpleHttpServer *http_server = static_cast<SimpleHttpServer*>(arg);
    http_server->handle_async_request(req);
    HTTPD_LOG(HTTPD_LOG_TRACE) << "Async handler called:" << std::endl;
}


void SimpleHttpServer::process_async_task(struct evhttp_request* req) {
    HTTPD_LOG(HTTPD_LOG_TRACE) << "DocRoot:" << docroot_ << std::endl;
    const char *uri = evhttp_request_get_uri(req);
    const char *path = NULL;
    char whole_path[2048];
    int fd = -1;
    struct stat st;
    std::string file_path_;
    char* decoded_path_;
    struct evhttp_uri *decoded_;
    struct evbuffer *evb_;

    if (evhttp_request_get_command(req) != EVHTTP_REQ_GET) {
        evhttp_send_error(req, HTTP_BADREQUEST, 0);
        return;
    }
    decoded_ = evhttp_uri_parse(uri);
    if (!decoded_) {
        HTTPD_LOG(HTTPD_LOG_TRACE)
                << "It's not a good URI. Sending BADREQUEST\n";
        evhttp_send_error(req, HTTP_BADREQUEST, 0);
        return;
    }
    path = evhttp_uri_get_path(decoded_);
    if (!path) {
        path = "/";
    }

    decoded_path_ = evhttp_uridecode(path, 0, NULL);
    if (decoded_path_ == NULL) {
        evhttp_send_error(req, HTTP_BADREQUEST, 0);
        return;
    }

    file_path_ = decoded_path_;
    file_path_ = file_path_.substr(6, file_path_.length());
    evutil_snprintf(whole_path, 2048, "%s/%s", docroot_.c_str(),
                    file_path_.c_str());
    if (stat(whole_path, &st)<0) {
        HTTPD_LOG(HTTPD_LOG_TRACE) << "filestat" << whole_path << " failed"
                << std::endl;
        evhttp_send_error(req, HTTP_NOTFOUND, 0);
        return;
    }
    evb_ = evbuffer_new();
    const char *type = guess_content_type(decoded_path_);
    if ((fd = open(whole_path, O_RDONLY)) < 0) {
        HTTPD_LOG(HTTPD_LOG_TRACE) << "fileopen" << whole_path << " failed"
                << std::endl;
        evhttp_send_error(req, HTTP_NOTFOUND, 0);
        return;
    }

    if (fstat(fd, &st) < 0) {
        /* Make sure the length still matches, now that we
         * opened the file :/ */
        evhttp_send_error(req, HTTP_NOTFOUND, 0);
        return;
    }
    evhttp_add_header(evhttp_request_get_output_headers(req),
        "Content-Type", type);
    evbuffer_add_file(evb_, fd, 0, st.st_size);

    evhttp_send_reply(req, 200, "OK", evb_);
    HTTPD_LOG(HTTPD_LOG_TRACE) << "sending file" << whole_path << std::endl;
    return;
}



void SimpleHttpServer::handle_async_request(struct evhttp_request * req) {
    GTaskQSharedPtr taskq = async_executor_->taskq();
    boost::shared_ptr<DeferredTask<void>> d(
            new DeferredTask<void>(
                    taskq,
                    boost::bind(&SimpleHttpServer::process_async_task,
                                this, req)));
    async_executor_->EnQueueTask(d);
}

void SimpleHttpServer::process_sync_task_callback(struct evhttp_request *req,
                                                  std::string whole_path) {
    int fd = -1;
    struct stat st;
    HTTPD_LOG(HTTPD_LOG_TRACE) << "sending response"
            << whole_path << std::endl;
    struct evbuffer *evb = evbuffer_new();

    if (stat(whole_path.c_str(), &st)<0) {
        HTTPD_LOG(HTTPD_LOG_TRACE) << "filestat" << whole_path << " failed"
                << std::endl;
        evhttp_send_error(req, HTTP_NOTFOUND, 0);
        return;
    }
    if ((fd = open(whole_path.c_str(), O_RDONLY)) < 0) {
        HTTPD_LOG(HTTPD_LOG_TRACE) << "fileopen" << whole_path << " failed"
                << std::endl;
        evhttp_send_error(req, HTTP_NOTFOUND, 0);
        return;
    }

    if (fstat(fd, &st) < 0) {
        /* Make sure the length still matches, now that we
         * opened the file :/ */
        evhttp_send_error(req, HTTP_NOTFOUND, 0);
        return;
    }
    HTTPD_LOG(HTTPD_LOG_TRACE) << "adding file to buf" << std::endl;
    evbuffer_add_file(evb, fd, 0, st.st_size);
    HTTPD_LOG(HTTPD_LOG_TRACE) << "sending reply" << std::endl;
    evhttp_send_reply(req, 200, "OK", evb);
    return;
}


std::string SimpleHttpServer::process_sync_task(struct evhttp_request *req) {
    std::string file_path_;
    char* decoded_path_ = NULL;
    struct evhttp_uri *decoded_  = NULL;

    HTTPD_LOG(HTTPD_LOG_TRACE) << "DocRoot:" << docroot_ << std::endl;

    if (req == NULL) {
        evhttp_send_error(req, HTTP_BADREQUEST, 0);
        return std::string("Failure");
    }

    const char *uri = evhttp_request_get_uri(req);
    const char *path = NULL;
    char whole_path[2048];

    if (evhttp_request_get_command(req) != EVHTTP_REQ_GET) {
        evhttp_send_error(req, HTTP_BADREQUEST, 0);
        HTTPD_LOG(HTTPD_LOG_TRACE)
                       << "It's not a good URI. Sending BADREQUEST\n";
        return std::string("Failure");
    }
    decoded_ = evhttp_uri_parse(uri);
    if (!decoded_) {
        HTTPD_LOG(HTTPD_LOG_TRACE)
                << "It's not a good URI. Sending BADREQUEST\n";
        evhttp_send_error(req, HTTP_BADREQUEST, 0);
        return std::string("Failure");
    }
    path = evhttp_uri_get_path(decoded_);
    if (!path) {
        path = "/";
    }

    decoded_path_ = evhttp_uridecode(path, 0, NULL);
    if (decoded_path_ == NULL) {
        evhttp_send_error(req, HTTP_BADREQUEST, 0);
        return std::string("Failure");
    }

    file_path_ = decoded_path_;
    file_path_ = file_path_.substr(5, file_path_.length());
    evutil_snprintf(whole_path, 2048, "%s/%s", docroot_.c_str(),
                    file_path_.c_str());
    HTTPD_LOG(HTTPD_LOG_TRACE) << "filepath" << whole_path << std::endl;

    const char *type = guess_content_type(decoded_path_);
    evhttp_add_header(evhttp_request_get_output_headers(req),
                      "Content-Type", type);

    /**
     * Instead of sending response here create a new ask to response back.
     */
    HTTPD_LOG(HTTPD_LOG_TRACE) << "filestat: " << whole_path
            << " sending response task" << std::endl;
    return std::string(whole_path);
}

void SimpleHttpServer::handle_sync_request(struct evhttp_request * req) {
    if (req == NULL) {
        HTTPD_ERR(HTTPD_LOG_ERROR) << "Reques is null:" << std::endl;
        return;
    }
    GTaskQSharedPtr taskq = sync_executor_->taskq();
    boost::shared_ptr<DeferredTask<std::string>> d(
            new DeferredTask<std::string>(
                    taskq,
                    boost::bind(&SimpleHttpServer::process_sync_task,
                                this, req)));
    d->set_callback(boost::bind(&SimpleHttpServer::process_sync_task_callback,
                               this, req, _1));
    sync_executor_->EnQueueTask(d);
}

void http_sync_request_cb(struct evhttp_request * req, void *arg) {
    SimpleHttpServer *http_server = static_cast<SimpleHttpServer*>(arg);
    http_server->handle_sync_request(req);
    HTTPD_LOG(HTTPD_LOG_TRACE) << "Sync handler called:" << std::endl;

}


void http_default_cb(struct evhttp_request * req, void *arg) {
    /**
     * 1. create a task to execute response in the sync queue.
     */
    const char *uri = evhttp_request_get_uri(req);

    std::string uri_str(uri);
    HTTPD_LOG(HTTPD_LOG_TRACE)
        << "uri: " << uri << std::endl;
    size_t pos = uri_str.find("/async");
    if ((pos != std::string::npos) && (pos == 0)) {
        return http_async_request_cb(req, arg);
    }
    pos = uri_str.find("/sync");
    if ((pos != std::string::npos) && (pos == 0)) {
        return http_sync_request_cb(req, arg);
    }

    SimpleHttpServer *http_server = static_cast<SimpleHttpServer*>(arg);
    const std::string& docroot = http_server->docroot();
    HTTPD_LOG(HTTPD_LOG_TRACE) << "DocRoot:" << docroot << std::endl;
    const char *path = NULL;
    char *decoded_path = NULL;
    char whole_path[2048];
    int fd = -1;
    struct stat st;
    struct evhttp_uri *decoded = NULL;
    struct evbuffer *evb = NULL;

    if (evhttp_request_get_command(req) != EVHTTP_REQ_GET) {
        evhttp_send_error(req, HTTP_BADREQUEST, 0);
        http_req_cleanup(decoded_path, decoded, evb);
        return;
    }
    decoded = evhttp_uri_parse(uri);
    if (!decoded) {
        HTTPD_LOG(HTTPD_LOG_TRACE)
                << "It's not a good URI. Sending BADREQUEST\n";
        evhttp_send_error(req, HTTP_BADREQUEST, 0);
        http_req_cleanup(decoded_path, decoded, evb);
        return;
    }
    path = evhttp_uri_get_path(decoded);
    if (!path) path = "/";
    decoded_path = evhttp_uridecode(path, 0, NULL);
    if (decoded_path == NULL) {
        evhttp_send_error(req, HTTP_BADREQUEST, 0);
        http_req_cleanup(decoded_path, decoded, evb);
        return;
    }
    evutil_snprintf(whole_path, 2048, "%s/%s", docroot.c_str(), decoded_path);
    if (stat(whole_path, &st)<0) {
        HTTPD_LOG(HTTPD_LOG_TRACE) << "filestat" << whole_path << " failed"
                << std::endl;
        evhttp_send_error(req, HTTP_NOTFOUND, 0);
        http_req_cleanup(decoded_path, decoded, evb);
        return;
    }
    evb = evbuffer_new();
    const char *type = guess_content_type(decoded_path);
    if ((fd = open(whole_path, O_RDONLY)) < 0) {
        HTTPD_LOG(HTTPD_LOG_TRACE) << "fileopen" << whole_path << " failed"
                << std::endl;
        evhttp_send_error(req, HTTP_NOTFOUND, 0);
        http_req_cleanup(decoded_path, decoded, evb);
        return;
    }

    if (fstat(fd, &st) < 0) {
        /* Make sure the length still matches, now that we
         * opened the fi                le :/ */
        evhttp_send_error(req, HTTP_NOTFOUND, 0);
        http_req_cleanup(decoded_path, decoded, evb);
        return;
    }
    evhttp_add_header(evhttp_request_get_output_headers(req),
        "Content-Type", type);
    evbuffer_add_file(evb, fd, 0, st.st_size);

    evhttp_send_reply(req, 200, "OK", evb);
    HTTPD_LOG(HTTPD_LOG_TRACE) << "sending file" << whole_path << std::endl;
    if (decoded)
        evhttp_uri_free(decoded);
    if (decoded_path)
        free(decoded_path);
    if (evb)
        evbuffer_free(evb);
    return;
}

int32_t SimpleHttpServer::Initialize() {
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

    async_taskq_->Initialize();
    sync_taskq_->Initialize();
    async_executor_->Initialize();
    sync_executor_->Initialize();

    return 0;
}

SimpleHttpServer::SimpleHttpServer(uint32_t num_sync_workers) :
        docroot_(FLAGS_docroot.c_str()), num_sync_workers_(num_sync_workers),
        event_base_(NULL), evhttp_(NULL),
        executors_(true),
        async_taskq_(), sync_taskq_(),
        async_executor_(), sync_executor_() {

    event_base_ = executors_.event_base();

    if (!event_base_) {
        HTTPD_LOG(HTTPD_LOG_ERROR) << "Event Base null"
                <<  "errno: " << errno << ": " << strerror(errno) << std::endl;
        return;
    }
    HTTPD_LOG(HTTPD_LOG_TRACE) << "Created Event Base\n";

    evhttp_ = evhttp_new(event_base_);

    if (!evhttp_) {
        HTTPD_LOG(HTTPD_LOG_ERROR) << "EvHTTP creation failed. "
                <<  "errno: " << errno << ": " << strerror(errno) << std::endl;
        return;
    }
    HTTPD_LOG(HTTPD_LOG_TRACE) << "Created HTTP Event \n";

    evhttp_set_gencb(evhttp_, http_default_cb, this);

    HTTPD_LOG(HTTPD_LOG_TRACE) << "Setup callbacks\n";

    HTTPD_LOG(HTTPD_LOG_TRACE) << "Creating http socket handle\n";

    async_executor_ =
            executors_.CreateAsyncExecutor(executors_.kDefaultExecutorId,
                                           GTaskQSharedPtr(NULL),
                                           event_base_);
    async_taskq_ = async_executor_->taskq();

    sync_executor_ = executors_.CreateSyncExecutor("sync", num_sync_workers);
    sync_taskq_ = sync_executor_->taskq();
}


