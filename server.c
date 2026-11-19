// Experiment : Multi-user chat server (TCP, one thread per client).
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "nw.h"

#define RMAX 1000
#define WMAX 2000
#define CMAX 100

typedef struct Connection Connection;
struct Connection {
    int fd;
    sockaddr_in addr;
    pthread_t thread_id;
};
Connection conn[CMAX];
int nconn = 0;

int xgets(char* data, int size, int cid) {
    int n = recv(conn[cid].fd, data, size - 1, 0);
    data[n > 0 ? n : 0] = '\0';
    return n;
}

int xputs(const char* data, int cid) {
    return send(conn[cid].fd, data, strlen(data), 0);
}

int processmain(int cid) {
    while (1) {
        char rline[RMAX];
        char wdata[WMAX];
        int status;

        status = xgets(rline, sizeof(rline), cid);
        if (status <= 0) return -1; // client left
        sprintf(wdata, "Client %d: %s", cid, rline);
        for (int i = 0; i < nconn; i++)
            if (conn[i].fd >= 0) // every connected client, sender included
                xputs(wdata, i);
    }
    return 0;
}

void* mythread(void* arg) {
    int cid = (int)(intptr_t)arg;

    processmain(cid); // serve this client until it leaves

    close(conn[cid].fd);
    conn[cid].fd = -1;
    printf("Client %d disconnected\n", cid);
    return NULL;
}

int main() {
    int fd = xsocket(SOCK_STREAM);
    xbind(fd, "0.0.0.0", 9999); // 0.0.0.0 = all local IP addresses
    listen(fd, 10);

    for (;;) {
        if (nconn == CMAX) return -1;
        int cid = nconn++;
        conn[cid].fd = xaccept(fd, &conn[cid].addr);
        printf("Client %d connected\n", cid);

        pthread_create(&conn[cid].thread_id, NULL, mythread, (void*)(intptr_t)cid);
        pthread_detach(conn[cid].thread_id);
    }
    return 0;
}
