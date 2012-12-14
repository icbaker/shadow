#include "shd-transport.h"
#include "utp.h"
#include "utp_utils.h"

static TransportClient* _transportutp_newClient(ShadowlibLogFunc log, in_addr_t serverIPAddress) {
    g_assert(log);

//    /* create the socket and get a socket descriptor */
//    gint socketd = socket(AF_INET, (SOCK_STREAM | SOCK_NONBLOCK), 0);
//    if (socketd == -1) {
//        log(G_LOG_LEVEL_WARNING, __FUNCTION__, "Error in socket");
//        return NULL;
//    }
//
//    /* setup the socket address info, client has outgoing connection to server */
//    struct sockaddr_in serverAddr;
//    memset(&serverAddr, 0, sizeof(serverAddr));
//    serverAddr.sin_family = AF_INET;
//    serverAddr.sin_addr.s_addr = serverIPAddress;
//    serverAddr.sin_port = htons(ECHO_SERVER_PORT);
//
//    /* connect to server. we cannot block, and expect this to return EINPROGRESS */
//    gint result = connect(socketd,(struct sockaddr *)  &serverAddr, sizeof(serverAddr));
//    if (result == -1 && errno != EINPROGRESS) {
//        log(G_LOG_LEVEL_WARNING, __FUNCTION__, "Error in connect");
//        return NULL;
//    }

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
