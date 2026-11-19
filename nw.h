#pragma once

// Network related Header Common for All programs

#include <stdio.h> // perror
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

typedef struct sockaddr sockaddr;
typedef struct sockaddr_in sockaddr_in;

int xsocket(int type){
    int fd=socket(AF_INET, type, 0);
    if(fd<0){ perror("Error"); exit(1); }
    return fd;
}

void xbind(int fd, const char* ip, int port){
    sockaddr_in addr= {
        .sin_family=AF_INET,
        .sin_port = htons(port),
        .sin_addr.s_addr = inet_addr(ip)
    };
    int status = bind(fd, (const sockaddr*) &addr, sizeof(addr));
    if(status<0){ perror("Error"); exit(1); }
}

void xconnect(int fd, const char* ip, int port) {
    sockaddr_in addr= {
        .sin_family=AF_INET,
        .sin_port = htons(port),
        .sin_addr.s_addr = inet_addr(ip)
    };
    int status = connect(fd, (const sockaddr*)&addr, sizeof(addr));
    if (status < 0) { perror("Error"); exit(1); }
}

int xaccept(int fd, sockaddr_in* addr) {
    socklen_t socklen = sizeof(sockaddr_in);
    int c = accept(fd, (sockaddr*)addr, &socklen);
    if (c < 0) { perror("Error"); return -1; }
    return c;
}

void xnonblock(int fd) {
    int fl = fcntl(fd, F_GETFL, 0);
    if( fl < 0) { perror("Error"); exit(1); }
    int status = fcntl(fd, F_SETFL, fl | O_NONBLOCK);
    if( status < 0) { perror("Error"); exit(1); }
}

void xreuse(int fd) {
    int tmp = 1;
    int status = setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &tmp, sizeof(tmp));
    if( status < 0) { perror("Error"); exit(1); }
}
