#include <fcntl.h>

#include "shd-transport.h"
#include "utp.h"
#include "utp_utils.h"

typedef void * SOCKOPTP;
typedef const void * CSOCKOPTP;

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

static TransportClient* _transportutp_newClient(ShadowlibLogFunc log, in_addr_t serverIPAddress) {
    g_assert(log);

    sockaddr_in sin;

    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = INADDR_ANY;
    sin.sin_port = htons(port);
    int sock = make_socket((const struct sockaddr*)&sin, sizeof(sin));

    //sm.set_socket(sock);

    char *portchr = strchr(dest, ':');
    *portchr = 0;
    portchr++;

    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = inet_addr(dest);
    sin.sin_port = htons(atoi(portchr));

    socket_state s;
    s.s = UTP_Create(&send_to, &sm, (const struct sockaddr*)&sin, sizeof(sin));
    UTP_SetSockopt(s.s, SO_SNDBUF, 100*300);
    s.state = 0;
    printf("creating socket %p\n", s.s);

    UTPFunctionTable utp_callbacks = {
        &utp_read,
        &utp_write,
        &utp_get_rb_size,
        &utp_state,
        &utp_error,
        &utp_overhead
    };
    g_sockets.Append(s);
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
    result = epoll_ctl(epolld, EPOLL_CTL_ADD, socketd, &ev);
    if(result == -1) {
        log(G_LOG_LEVEL_WARNING, __FUNCTION__, "Error in epoll_ctl");
        close(epolld);
        close(socketd);
        return NULL;
    }

    /* create our client and store our client socket */
    TransportClient* tc = g_new0(EchoClient, 1);
    tc->socketd = socketd;
    tc->epolld = epolld;
    tc->serverIP = serverIPAddress;
    tc->log = log;
    return tc;
}
