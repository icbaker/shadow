#ifndef SHD_TRANSPORT_H_
#define SHD_TRANSPORT_H_

#include <glib.h>
#include <shd-library.h>
#include "utp.h"

#include <bits/socket.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <errno.h>
#include <unistd.h>

#define BUFFERSIZE 20000
#define TRANSPORT_SERVER_PORT 9999
#define MAX_EVENTS 10

struct socket_state
{
    int total_sent;
    int state;
    struct UTPSocket* s;
};

/**
 * Protocol modes this transport module supports.
 */
enum TransportProtocol {
    TRANSPORTP_UTP,
};

/**
 *
 */
typedef struct _TransportClient TransportClient;
struct _TransportClient {
    ShadowlibLogFunc log;
    in_addr_t serverIP;
    gint epolld;
    gint socketd;
    struct socket_state* utpSockState;
    gchar sendBuffer[BUFFERSIZE];
    gchar recvBuffer[BUFFERSIZE];
    gint recv_offset;
    gint sent_msg;
    gint is_done;
};

/**
 *
 */
typedef struct _TransportServer TransportServer;
struct _TransportServer {
    ShadowlibLogFunc log;
    gint epolld;
    gint listend;
    gint socketd;
    struct socket_state* utpSockState;
    struct sockaddr_in address;
    gchar echoBuffer[BUFFERSIZE];
    gint read_offset;
    gint write_offset;
};

/**
 *
 */
typedef struct _TransportUTP TransportUTP;
struct _TransportUTP {
    ShadowlibLogFunc log;
    TransportClient* client;
    TransportServer* server;
    UTPGlobalState* utp_state;
};

/**
 *
 */
typedef struct _Transport Transport;
struct _Transport {
    ShadowlibFunctionTable shadowlibFuncs;
    enum TransportProtocol protocol;
    TransportUTP* tutp;
};

void transportplugin_new(int argc, char* argv[]);
void transportplugin_free();
void transportplugin_ready();

TransportUTP* transportutp_new(ShadowlibLogFunc log, int argc, char* argv[]);
void transportutp_free(TransportUTP* tutp);
void transportutp_ready(TransportUTP* tutp);

#endif /* SHD_TRANSPORT_H_ */
