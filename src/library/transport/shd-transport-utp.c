#include <fcntl.h>

#include "shd-transport.h"

typedef void * SOCKOPTP;
typedef const void * CSOCKOPTP;

int g_send_limit = 50 * 1024 * 1024;

int make_socket(const struct sockaddr *addr, socklen_t addrlen)
{
    int s = socket(addr->sa_family, SOCK_DGRAM, 0);
    if (s == -1) return s;

    // make socket non blocking
    int flags = fcntl(s, F_GETFL, 0);
    fcntl(s, F_SETFL, flags | O_NONBLOCK);

    return s;
}

void send_to(void *userdata, const byte *p, size_t len, const struct sockaddr *to, socklen_t tolen) {
    sendto(*(int*)userdata, (char*)p, len, 0, (struct sockaddr*)to, tolen);
}

void utp_read(void* transportUtp, const byte* bytes, size_t count)
{
    TransportUTP* tutp = (TransportUTP*) transportUtp;
    int i = 0;
    if (tutp->client) {
        for (i = 0; i < count; i++) {
            tutp->client->recvBuffer[tutp->client->recv_offset + i] = bytes[i];
        }
        tutp->client->recv_offset += count;
    } else if (tutp->server) {
        for (i = 0; i < count; i++) {
            tutp->server->echoBuffer[tutp->server->read_offset + i] = bytes[i];
        }
        tutp->server->read_offset += count;
    }
}

void utp_write(void* transportUtp, byte* bytes, size_t count)
{
    TransportUTP* tutp = (TransportUTP*) transportUtp;
    if (tutp->client) {
        memcpy(bytes, tutp->client->sendBuffer + tutp->client->utpSockState->total_sent, count);
        tutp->client->utpSockState->total_sent += count;
        tutp->client->log(G_LOG_LEVEL_INFO, __FUNCTION__, "client UTP socket %i wrote %i bytes", tutp->client->utpSockState->s, count);
        tutp->client->sent_msg = 1;
    } else if (tutp->server) {
        memcpy(bytes, tutp->server->echoBuffer + tutp->server->write_offset, count);
        tutp->server->write_offset += count;
        tutp->server->log(G_LOG_LEVEL_INFO, __FUNCTION__, "server UTP socket %i wrote %i bytes", tutp->server->utpSockState->s, count);
    }
}

size_t utp_get_rb_size(void* transportUtp)
{
    TransportUTP* tutp = (TransportUTP*) transportUtp;
    if (tutp->client) {
        return tutp->client->recv_offset;
    } else if (tutp->server) {
        return tutp->server->read_offset;
    } else {
        return 0;
    }
}

void utp_state(void* transportUtp, int state)
{
    TransportUTP* tutp = (TransportUTP*) transportUtp;
    UTP_UpdateGlobalState(tutp->utp_state);
    struct socket_state* s;
    if (tutp->client) {
        tutp->client->utpSockState->state = state;
        s = tutp->client->utpSockState;
    } else if (tutp->server) {
        tutp->server->utpSockState->state = state;
        s = tutp->server->utpSockState;
    }
    if (state == UTP_STATE_WRITABLE || state == UTP_STATE_CONNECT) {
        if (UTP_Write(s->s, g_send_limit - s->total_sent)) {
            UTP_Close(s->s);
            s->s = NULL;
        }
    }
}

void utp_error(void* transportUtp, int errcode)
{
    TransportUTP* tutp = (TransportUTP*) transportUtp;
    UTP_UpdateGlobalState(tutp->utp_state);
    printf("socket error: (%d) %s\n", errcode, strerror(errcode));
    struct socket_state* s;
    if (tutp->client->utpSockState) {
        s = tutp->client->utpSockState;
    } else if (tutp->server->utpSockState) {
        s = tutp->server->utpSockState;
    }
    UTP_Close(s->s);
    s->s = NULL;
}

void utp_overhead(void *transportUtp, bool send, size_t count, int type)
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
    tc->utpSockState = &s;
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
               inet_ntop(sin.sin_family, (struct sockaddr*)&sin, str, sizeof(str)), errno, strerror(errno));
        close(socketd);
        return NULL;
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
    ts->utpSockState = &s;
    return ts;
}

TransportUTP* transportutp_new(ShadowlibLogFunc log, int argc, char* argv[]) {
    g_assert(log);

    if(argc < 1) {
        return NULL;
    }

    TransportUTP* tutp = g_new0(TransportUTP, 1);
    tutp->log = log;

    tutp->utp_state = UTP_AllocGlobalState();
    UTP_UpdateGlobalState(tutp->utp_state);

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

    UTP_FreeGlobalState(tutp->utp_state);
    g_free(tutp);
}

static void _transportutp_clientReadable(TransportClient* tc, gint socketd) {
    tc->log(G_LOG_LEVEL_DEBUG, __FUNCTION__, "trying to read socket %i", socketd);

    if(!tc->is_done) {
        ssize_t b = 0;
        while(tc->utpSockState->total_sent-tc->recv_offset > 0 &&
                (b = recvfrom(socketd, tc->recvBuffer+tc->recv_offset, tc->utpSockState->total_sent-tc->recv_offset, 0, NULL, NULL)) > 0) {
            tc->log(G_LOG_LEVEL_DEBUG, __FUNCTION__, "client socket %i read %i bytes: '%s'", socketd, b, tc->recvBuffer+tc->recv_offset);
            tc->recv_offset += b;
            UTP_IsIncomingUTP(NULL, &send_to, &socketd, tc->recvBuffer+tc->recv_offset, b, (const struct sockaddr*)&tc->serverIP, sizeof(tc->serverIP));
        }

        if(tc->recv_offset >= tc->utpSockState->total_sent) {
            tc->is_done = 1;
            if(memcmp(tc->sendBuffer, tc->recvBuffer, tc->utpSockState->total_sent)) {
                tc->log(G_LOG_LEVEL_MESSAGE, __FUNCTION__, "inconsistent transmission received!");
            } else {
                tc->log(G_LOG_LEVEL_MESSAGE, __FUNCTION__, "consistent transmission received!");
            }

            if(epoll_ctl(tc->epolld, EPOLL_CTL_DEL, socketd, NULL) == -1) {
                tc->log(G_LOG_LEVEL_WARNING, __FUNCTION__, "Error in epoll_ctl");
            }

            close(socketd);
        } else {
            tc->log(G_LOG_LEVEL_INFO, __FUNCTION__, "transmission progress: %i of %i bytes", tc->recv_offset, tc->utpSockState->total_sent);
        }
    }
}

static void _transportutp_serverReadable(TransportServer* ts, gint socketd) {
    ts->log(G_LOG_LEVEL_DEBUG, __FUNCTION__, "trying to read socket %i", socketd);

    socklen_t len = sizeof(ts->address);

    /* read all data available */
    gint read_size = BUFFERSIZE - ts->read_offset;
    if(read_size > 0) {
        ssize_t bread = recvfrom(socketd, ts->echoBuffer + ts->read_offset, read_size, 0, (struct sockaddr*)&ts->address, &len);

        /* if we read, start listening for when we can write */
        if(bread == 0) {
            close(ts->listend);
            close(socketd);
        } else if(bread > 0) {
            ts->log(G_LOG_LEVEL_INFO, __FUNCTION__, "server socket %i read %i bytes", socketd, (gint)bread);
            ts->read_offset += bread;
            read_size -= bread;
            UTP_IsIncomingUTP(NULL, &send_to, &socketd, ts->echoBuffer+ts->read_offset, bread, (struct sockaddr*)&ts->address, sizeof(ts->address));

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
static void _transportutp_fillCharBuffer(byte* buffer, gint size) {
    for(gint i = 0; i < size; i++) {
        gint n = rand() % 26;
        buffer[i] = 'a' + n;
    }
}

static void _transportutp_clientWritable(TransportClient* tc, gint socketd) {
    if(!tc->sent_msg) {
        tc->log(G_LOG_LEVEL_DEBUG, __FUNCTION__, "trying to write to socket %i", socketd);

        _transportutp_fillCharBuffer(tc->sendBuffer, sizeof(tc->sendBuffer)-1);

        UTP_Write(tc->utpSockState->s, sizeof(tc->sendBuffer));
        tc->log(G_LOG_LEVEL_INFO, __FUNCTION__, "client UTP socket %i asked to write %i bytes: '%s'", socketd, sizeof(tc->sendBuffer));

        if(tc->utpSockState->total_sent >= sizeof(tc->sendBuffer)) {
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
    ts->log(G_LOG_LEVEL_DEBUG, __FUNCTION__, "trying to write to socket %i", socketd);

    /* echo it back to the client on the same sd,
     * also taking care of data that is still hanging around from previous reads. */
    gint write_size = ts->read_offset - ts->write_offset;
    ts->log(G_LOG_LEVEL_DEBUG, __FUNCTION__, "Write size is: %d", write_size);
    if(write_size > 0) {
        UTP_Write(ts->utpSockState->s, write_size);
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

    tutp->log(G_LOG_LEVEL_DEBUG, __FUNCTION__, "Entered transportutp_ready");

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
