#include <fcntl.h>

#include "shd-transport.h"
#include "utp.h"

typedef void * SOCKOPTP;
typedef const void * CSOCKOPTP;

int g_send_limit = 50 * 1024 * 1024;
int g_total_sent = 0;

struct socket_state
{
    int total_sent;
    int state;
    struct UTPSocket* s;
};

int make_socket(const struct sockaddr *addr, socklen_t addrlen)
{
    int s = socket(addr->sa_family, SOCK_DGRAM, 0);
    if (s == -1) return s;

    // Mark to hold a couple of megabytes
    int size = 2 * 1024 * 1024;

    // make socket non blocking
    int flags = fcntl(s, F_GETFL, 0);
    fcntl(s, F_SETFL, flags | O_NONBLOCK);

    return s;
}

void send_to(void *userdata, const byte *p, size_t len, const struct sockaddr *to, socklen_t tolen) {
    sendto(*(int*)userdata, (char*)p, len, 0, (struct sockaddr*)to, tolen);
}

void utp_read(void* socket, const byte* bytes, size_t count)
{
}

void utp_write(void* socket, byte* bytes, size_t count)
{
    for (size_t i = 0; i < count; ++i) {
        *bytes = rand();
        ++bytes;
    }
    struct socket_state* s = (struct socket_state*)socket;
    s->total_sent += count;
    g_total_sent += count;
}

size_t utp_get_rb_size(void* socket)
{
    return 0;
}

void utp_state(void* socket, int state)
{
    struct socket_state* s = (struct socket_state*)socket;
    s->state = state;
    if (state == UTP_STATE_WRITABLE || state == UTP_STATE_CONNECT) {
        if (UTP_Write(s->s, g_send_limit - s->total_sent)) {
            UTP_Close(s->s);
            s->s = NULL;
        }
    }
}

void utp_error(void* socket, int errcode)
{
    struct socket_state* s = (struct socket_state*)socket;
    printf("socket error: (%d) %s\n", errcode, strerror(errcode));
    if (s->s) {
        UTP_Close(s->s);
        s->s = NULL;
    }
}

void utp_overhead(void *socket, bool send, size_t count, int type)
{
}

static TransportClient* _transportutp_newClient(ShadowlibLogFunc log, in_addr_t serverIPAddress) {
    g_assert(log);

    struct sockaddr_in sin;

    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = serverIPAddress;
    sin.sin_port = htons(TRANSPORT_SERVER_PORT);
    int socketd = make_socket((const struct sockaddr*)&sin, sizeof(sin));

    struct socket_state s;
    s.s = UTP_Create(&send_to, &socketd, (const struct sockaddr*)&sin, sizeof(sin));
    UTP_SetSockopt(s.s, SO_SNDBUF, 100*300);
    s.state = 0;
    printf("Client: creating UTP socket %p\n", s.s);

    struct UTPFunctionTable utp_callbacks = {
        &utp_read,
        &utp_write,
        &utp_get_rb_size,
        &utp_state,
        &utp_error,
        &utp_overhead
    };
    UTP_SetCallbacks(s.s, &utp_callbacks, &s);

    printf("Client: connecting UTP socket %p\n", s.s);
    UTP_Connect(s.s);

    /* create an epoll so we can wait for IO events */
    gint epolld = epoll_create(1);
    if(epolld == -1) {
        log(G_LOG_LEVEL_WARNING, __FUNCTION__, "Error in epoll_create");
        close(socketd);
        return NULL;
    }

    /* setup the events we will watch for */
    struct epoll_event ev;
    ev.events = EPOLLIN|EPOLLOUT;
    ev.data.fd = socketd;

    /* start watching our socket */
    gint result = epoll_ctl(epolld, EPOLL_CTL_ADD, socketd, &ev);
    if(result == -1) {
        log(G_LOG_LEVEL_WARNING, __FUNCTION__, "Error in epoll_ctl");
        close(epolld);
        close(socketd);
        return NULL;
    }

    /* create our client and store our client socket */
    TransportClient* tc = g_new0(TransportClient, 1);
    tc->socketd = socketd;
    tc->epolld = epolld;
    tc->serverIP = serverIPAddress;
    tc->log = log;
    return tc;
}

static TransportServer* _transportutp_newServer(ShadowlibLogFunc log, in_addr_t bindIPAddress) {
    g_assert(log);

    struct sockaddr_in sin;

    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = bindIPAddress;
    sin.sin_port = htons(TRANSPORT_SERVER_PORT);
    int socketd = make_socket((const struct sockaddr*)&sin, sizeof(sin));

    if (bind(socketd, &sin, sizeof(sin)) < 0) {
        char str[20];
        printf("UDP port bind failed %s: (%d) %s\n",
               inet_ntop(&sin.sin_family, (struct sockaddr*)&sin, str, sizeof(str)), errno, strerror(errno));
        close(socketd);
        return -1;
    }

    struct socket_state s;
    s.s = UTP_Create(&send_to, &socketd, (const struct sockaddr*)&sin, sizeof(sin));
    UTP_SetSockopt(s.s, SO_SNDBUF, 100*300);
    s.state = 0;
    printf("Server: creating UTP socket %p\n", s.s);

    struct UTPFunctionTable utp_callbacks = {
        &utp_read,
        &utp_write,
        &utp_get_rb_size,
        &utp_state,
        &utp_error,
        &utp_overhead
    };
    UTP_SetCallbacks(s.s, &utp_callbacks, &s);

    printf("Server: connecting UTP socket %p\n", s.s);
    UTP_Connect(s.s);

    /* create an epoll so we can wait for IO events */
    gint epolld = epoll_create(1);
    if(epolld == -1) {
        log(G_LOG_LEVEL_WARNING, __FUNCTION__, "Error in epoll_create");
        close(socketd);
        return NULL;
    }

    /* setup the events we will watch for */
    struct epoll_event ev;
    ev.events = EPOLLIN;
    ev.data.fd = socketd;

    /* start watching out socket */
    gint result = epoll_ctl(epolld, EPOLL_CTL_ADD, socketd, &ev);
    if(result == -1) {
        log(G_LOG_LEVEL_WARNING, __FUNCTION__, "Error in epoll_ctl");
        close(epolld);
        close(socketd);
        return NULL;
    }

    /* create our server and store our server socket */
    TransportServer* ts = g_new0(TransportServer, 1);
    ts->listend = socketd;
    ts->epolld = epolld;
    ts->log = log;
    return ts;
}

TransportUTP* transportutp_new(ShadowlibLogFunc log, int argc, char* argv[]) {
    g_assert(log);

    if(argc < 1) {
        return NULL;
    }

    TransportUTP* tutp = g_new0(TransportUTP, 1);
    tutp->log = log;

    gchar* mode = argv[0];
    gboolean isError = FALSE;

    if(g_strncasecmp(mode, "client", 6) == 0)
    {
        if(argc < 2) {
            isError = TRUE;
        } else {
            gchar* serverHostName = argv[1];
            struct addrinfo* serverInfo;

            if(getaddrinfo(serverHostName, NULL, NULL, &serverInfo) == -1) {
                log(G_LOG_LEVEL_WARNING, __FUNCTION__, "unable to create client: error in getaddrinfo");
                isError = TRUE;
            } else {
                in_addr_t serverIP = ((struct sockaddr_in*)(serverInfo->ai_addr))->sin_addr.s_addr;
                tutp->client = _transportutp_newClient(log, serverIP);
            }
            freeaddrinfo(serverInfo);
        }
    }
    else if (g_strncasecmp(mode, "server", 6) == 0)
    {
        char myHostName[128];

        gint result = gethostname(myHostName, 128);
        if(result == 0) {
            struct addrinfo* myInfo;

            if(getaddrinfo(myHostName, NULL, NULL, &myInfo) == -1) {
                log(G_LOG_LEVEL_WARNING, __FUNCTION__, "unable to create server: error in getaddrinfo");
                isError = TRUE;
            } else {
                in_addr_t myIP = ((struct sockaddr_in*)(myInfo->ai_addr))->sin_addr.s_addr;
                log(G_LOG_LEVEL_INFO, __FUNCTION__, "binding to %s", inet_ntoa((struct in_addr){myIP}));
                tutp->server = _transportutp_newServer(log, myIP);
            }
            freeaddrinfo(myInfo);
        } else {
            log(G_LOG_LEVEL_WARNING, __FUNCTION__, "unable to create server: error in gethostname");
            isError = TRUE;
        }
    }
    else if (g_strncasecmp(mode, "loopback", 8) == 0)
    {
        in_addr_t serverIP = htonl(INADDR_LOOPBACK);
        tutp->server = _transportutp_newServer(log, serverIP);
        tutp->client = _transportutp_newClient(log, serverIP);
    }
    else {
        isError = TRUE;
    }

    if(isError) {
        g_free(tutp);
        return NULL;
    }

    return tutp;
}

void transportutp_free(TransportUTP* tutp) {
    g_assert(tutp);

    if(tutp->client) {
        epoll_ctl(tutp->client->epolld, EPOLL_CTL_DEL, tutp->client->socketd, NULL);
        g_free(tutp->client);
    }

    if(tutp->server) {
        epoll_ctl(tutp->server->epolld, EPOLL_CTL_DEL, tutp->server->listend, NULL);
        g_free(tutp->server);
    }

    g_free(tutp);
}

static void _transportutp_clientReadable(TransportClient* tc, gint socketd) {
    tc->log(G_LOG_LEVEL_DEBUG, __FUNCTION__, "trying to read socket %i", socketd);

    if(!tc->is_done) {
        ssize_t b = 0;
        while(tc->amount_sent-tc->recv_offset > 0 &&
                (b = recvfrom(socketd, tc->recvBuffer+tc->recv_offset, tc->amount_sent-tc->recv_offset, 0, NULL, NULL)) > 0) {
            tc->log(G_LOG_LEVEL_DEBUG, __FUNCTION__, "client socket %i read %i bytes: '%s'", socketd, b, tc->recvBuffer+tc->recv_offset);
            tc->recv_offset += b;
        }

        if(tc->recv_offset >= tc->amount_sent) {
            tc->is_done = 1;
            if(memcmp(tc->sendBuffer, tc->recvBuffer, tc->amount_sent)) {
                tc->log(G_LOG_LEVEL_MESSAGE, __FUNCTION__, "inconsistent transmission received!");
            } else {
                tc->log(G_LOG_LEVEL_MESSAGE, __FUNCTION__, "consistent transmission received!");
            }

            if(epoll_ctl(tc->epolld, EPOLL_CTL_DEL, socketd, NULL) == -1) {
                tc->log(G_LOG_LEVEL_WARNING, __FUNCTION__, "Error in epoll_ctl");
            }

            close(socketd);
        } else {
            tc->log(G_LOG_LEVEL_INFO, __FUNCTION__, "transmission progress: %i of %i bytes", tc->recv_offset, tc->amount_sent);
        }
    }
}

static void _transportutp_serverReadable(TransportServer* ts, gint socketd) {
    ts->log(G_LOG_LEVEL_DEBUG, __FUNCTION__, "trying to read socket %i", socketd);

    socklen_t len = sizeof(ts->address);

    /* read all data available */
    gint read_size = BUFFERSIZE - ts->read_offset;
    if(read_size > 0) {
        ssize_t bread = recvfrom(socketd, ts->transportBuffer + ts->read_offset, read_size, 0, (struct sockaddr*)&ts->address, &len);

        /* if we read, start listening for when we can write */
        if(bread == 0) {
            close(ts->listend);
            close(socketd);
        } else if(bread > 0) {
            ts->log(G_LOG_LEVEL_INFO, __FUNCTION__, "server socket %i read %i bytes", socketd, (gint)bread);
            ts->read_offset += bread;
            read_size -= bread;

            struct epoll_event ev;
            ev.events = EPOLLIN|EPOLLOUT;
            ev.data.fd = socketd;
            if(epoll_ctl(ts->epolld, EPOLL_CTL_MOD, socketd, &ev) == -1) {
                ts->log(G_LOG_LEVEL_WARNING, __FUNCTION__, "Error in epoll_ctl");
            }
        }
    }
}

/* fills buffer with size random characters */
static void _transportutp_fillCharBuffer(gchar* buffer, gint size) {
    for(gint i = 0; i < size; i++) {
        gint n = rand() % 26;
        buffer[i] = 'a' + n;
    }
}

static void _transportutp_clientWritable(TransportClient* tc, gint socketd) {
    if(!tc->sent_msg) {
        tc->log(G_LOG_LEVEL_DEBUG, __FUNCTION__, "trying to write to socket %i", socketd);

        struct sockaddr_in server;
        memset(&server, 0, sizeof(server));
        server.sin_family = AF_INET;
        server.sin_addr.s_addr = tc->serverIP;
        server.sin_port = htons(TRANSPORT_SERVER_PORT);

        socklen_t len = sizeof(server);

        _transportutp_fillCharBuffer(tc->sendBuffer, sizeof(tc->sendBuffer)-1);

        ssize_t b = sendto(socketd, tc->sendBuffer, sizeof(tc->sendBuffer), 0, (struct sockaddr*) (&server), len);
        tc->sent_msg = 1;
        tc->amount_sent += b;
        tc->log(G_LOG_LEVEL_DEBUG, __FUNCTION__, "client socket %i wrote %i bytes: '%s'", socketd, b, tc->sendBuffer);

        if(tc->amount_sent >= sizeof(tc->sendBuffer)) {
            /* we sent everything, so stop trying to write */
            struct epoll_event ev;
            ev.events = EPOLLIN;
            ev.data.fd = socketd;
            if(epoll_ctl(tc->epolld, EPOLL_CTL_MOD, socketd, &ev) == -1) {
                tc->log(G_LOG_LEVEL_WARNING, __FUNCTION__, "Error in epoll_ctl");
            }
        }
    }
}

static void _transportutp_serverWritable(TransportServer* ts, gint socketd) {
    ts->log(G_LOG_LEVEL_DEBUG, __FUNCTION__, "trying to read socket %i", socketd);

    socklen_t len = sizeof(ts->address);

    /* echo it back to the client on the same sd,
     * also taking care of data that is still hanging around from previous reads. */
    gint write_size = ts->read_offset - ts->write_offset;
    if(write_size > 0) {
        ssize_t bwrote = sendto(socketd, ts->transportBuffer + ts->write_offset, write_size, 0, (struct sockaddr*)&ts->address, len);
        if(bwrote == 0) {
            if(epoll_ctl(ts->epolld, EPOLL_CTL_DEL, socketd, NULL) == -1) {
                ts->log(G_LOG_LEVEL_WARNING, __FUNCTION__, "Error in epoll_ctl");
            }
        } else if(bwrote > 0) {
            ts->log(G_LOG_LEVEL_INFO, __FUNCTION__, "server socket %i wrote %i bytes", socketd, (gint)bwrote);
            ts->write_offset += bwrote;
            write_size -= bwrote;
        }
    }

    if(write_size == 0) {
        /* stop trying to write */
        struct epoll_event ev;
        ev.events = EPOLLIN;
        ev.data.fd = socketd;
        if(epoll_ctl(ts->epolld, EPOLL_CTL_MOD, socketd, &ev) == -1) {
            ts->log(G_LOG_LEVEL_WARNING, __FUNCTION__, "Error in epoll_ctl");
        }
    }
}

void transportutp_ready(TransportUTP* tutp) {
    g_assert(tutp);

    if(tutp->client) {
        struct epoll_event events[MAX_EVENTS];

        int nfds = epoll_wait(tutp->client->epolld, events, MAX_EVENTS, 0);
        if(nfds == -1) {
            tutp->log(G_LOG_LEVEL_WARNING, __FUNCTION__, "error in epoll_wait");
        }

        for(int i = 0; i < nfds; i++) {
            if(events[i].events & EPOLLIN) {
                _transportutp_clientReadable(tutp->client, events[i].data.fd);
            }
            if(!tutp->client->is_done && (events[i].events & EPOLLOUT)) {
                _transportutp_clientWritable(tutp->client, events[i].data.fd);
            }
        }
    }

    if(tutp->server) {
        struct epoll_event events[MAX_EVENTS];

        int nfds = epoll_wait(tutp->server->epolld, events, MAX_EVENTS, 0);
        if(nfds == -1) {
            tutp->log(G_LOG_LEVEL_WARNING, __FUNCTION__, "error in epoll_wait");
        }

        for(int i = 0; i < nfds; i++) {
            if(events[i].events & EPOLLIN) {
                _transportutp_serverReadable(tutp->server, events[i].data.fd);
            }
            if(events[i].events & EPOLLOUT) {
                _transportutp_serverWritable(tutp->server, events[i].data.fd);
            }
        }

        if(tutp->server->read_offset == tutp->server->write_offset) {
            tutp->server->read_offset = 0;
            tutp->server->write_offset = 0;
        }

        /* cant close sockd to client if we havent received everything yet.
         * keep it simple and just keep the socket open for now.
         */
    }
}
