#include <fcntl.h>

#include "shd-transport.h"
#include "utp.h"
#include "utp_utils.h"

typedef void * SOCKOPTP;
typedef const void * CSOCKOPTP;

struct socket_state
{
    int total_sent;
    int state;
    UTPSocket* s;
};

int make_socket(const struct sockaddr *addr, socklen_t addrlen)
{
    int s = socket(addr->sa_family, SOCK_DGRAM, 0);
    if (s == -1) return s;

    if (bind(s, addr, addrlen) < 0) {
        char str[20];
        printf("UDP port bind failed %s: (%d) %s\n",
               inet_ntop(addr->sa_family, (sockaddr*)addr, str, sizeof(str)), errno, strerror(errno));
        closesocket(s);
        return -1;
    }

    // Mark to hold a couple of megabytes
    int size = 2 * 1024 * 1024;

    if (setsockopt(s, SOL_SOCKET, SO_RCVBUF, (CSOCKOPTP)&size, sizeof(size)) < 0) {
        printf("UDP setsockopt(SO_RCVBUF, %d) failed: %d %s\n", size, errno, strerror(errno));
    }
    if (setsockopt(s, SOL_SOCKET, SO_SNDBUF, (CSOCKOPTP)&size, sizeof(size)) < 0) {
        printf("UDP setsockopt(SO_SNDBUF, %d) failed: %d %s\n", size, errno, strerror(errno));
    }

    // make socket non blocking
    int flags = fcntl(s, F_GETFL, 0);
    fcntl(s, F_SETFL, flags | O_NONBLOCK);

    return s;
}

void send(int socket, const byte *p, size_t len, const struct sockaddr *to, socklen_t tolen) {
    sendto(socket, (char*)p, len, 0, (struct sockaddr*)to, tolen);
}

void send_to(void *userdata, const byte *p, size_t len, const struct sockaddr *to, socklen_t tolen) {
    send((int)*userdata, p, len, to, tolen);
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
    utp_log("[%p] state: %d", socket, state);

    struct socket_state* s = (struct socket_state*)socket;
    s->state = state;
    if (state == UTP_STATE_WRITABLE || state == UTP_STATE_CONNECT) {
        if (UTP_Write(s->s, g_send_limit - s->total_sent)) {
            UTP_Close(s->s);
            s->s = NULL;
        }
    } else if (state == UTP_STATE_DESTROYING) {
        size_t index = g_sockets.LookupElement(*s);
        if (index != (size_t)-1) g_sockets.MoveUpLast(index);
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
    int socketd = malloc(sizeof(int));
    socketd = make_socket((const struct sockaddr*)&sin, sizeof(sin));

    struct socket_state s;
    s.s = UTP_Create(&send_to, &socketd, (const struct sockaddr*)&sin, sizeof(sin));
    UTP_SetSockopt(s.s, SO_SNDBUF, 100*300);
    s.state = 0;
    printf("creating socket %p\n", s.s);

    struct UTPFunctionTable utp_callbacks = {
        &utp_read,
        &utp_write,
        &utp_get_rb_size,
        &utp_state,
        &utp_error,
        &utp_overhead
    };
    //g_sockets.Append(s);
    UTP_SetCallbacks(s.s, &utp_callbacks, &g_sockets[g_sockets.GetCount()-1]);

    printf("connecting socket %p\n", s.s);
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
        sin.sin_addr.s_addr = INADDR_ANY;
        sin.sin_port = htons(TRANSPORT_SERVER_PORT);
        int socketd = malloc(sizeof(int));
        socketd = make_socket((const struct sockaddr*)&sin, sizeof(sin));

        struct socket_state s;
        s.s = UTP_Create(&send_to, &socketd, (const struct sockaddr*)&sin, sizeof(sin));
        UTP_SetSockopt(s.s, SO_SNDBUF, 100*300);
        s.state = 0;
        printf("creating socket %p\n", s.s);

        struct UTPFunctionTable utp_callbacks = {
            &utp_read,
            &utp_write,
            &utp_get_rb_size,
            &utp_state,
            &utp_error,
            &utp_overhead
        };

    /* create the socket and get a socket descriptor */
    gint socketd = socket(AF_INET, (SOCK_DGRAM | SOCK_NONBLOCK), 0);
    if (socketd == -1) {
        log(G_LOG_LEVEL_WARNING, __FUNCTION__, "Error in socket");
        return NULL;
    }

    /* setup the socket address info, client has outgoing connection to server */
    struct sockaddr_in bindAddr;
    memset(&bindAddr, 0, sizeof(bindAddr));
    bindAddr.sin_family = AF_INET;
    bindAddr.sin_addr.s_addr = bindIPAddress;
    bindAddr.sin_port = htons(TRANSPORT_SERVER_PORT);

    /* bind the socket to the server port */
    gint result = bind(socketd, (struct sockaddr *) &bindAddr, sizeof(bindAddr));
    if (result == -1) {
        log(G_LOG_LEVEL_WARNING, __FUNCTION__, "error in bind");
        return NULL;
    }

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
    result = epoll_ctl(epolld, EPOLL_CTL_ADD, socketd, &ev);
    if(result == -1) {
        log(G_LOG_LEVEL_WARNING, __FUNCTION__, "Error in epoll_ctl");
        close(epolld);
        close(socketd);
        return NULL;
    }

    /* create our server and store our server socket */
    EchoServer* es = g_new0(EchoServer, 1);
    es->listend = socketd;
    es->epolld = epolld;
    es->log = log;
    return es;
}