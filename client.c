// Chat client: two things happen at the same time.
//   main thread   : reads lines from the keyboard and sends them (processmain)
//   second thread : receives messages from the server and prints them
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "nw.h"

#define RMAX 1000
#define WMAX 1000
#define CMAX 1 // the client has one connection: the server

typedef struct Connection Connection;
struct Connection {
    int fd;
    pthread_t thread_id;
};
Connection conn[CMAX];

int xgets(char* data, int size, int cid) {
    int n = recv(conn[cid].fd, data, size - 1, 0);
    data[n > 0 ? n : 0] = '\0';
    return n;
}

int xputs(const char* data, int cid) {
    return send(conn[cid].fd, data, strlen(data), 0);
}

// keyboard -> server
int processmain(int cid) {
    while (1) {
        char wdata[WMAX];
        char* line;

        line = fgets(wdata, sizeof(wdata), stdin);
        if (line == NULL) return -1; // Ctrl-D: end of keyboard input

        xputs(wdata, cid);
    }
    return 0;
}
// server -> screen
void* mythread(void* arg) {
    int cid = (int)(intptr_t)arg;

    while (1) {
        char rline[RMAX];
        int status;

        status = xgets(rline, sizeof(rline), cid);
        if (status <= 0) break; // server closed the connection

        fputs(rline, stdout);
        fflush(stdout);
    }

    printf("Connection closed\n");
    exit(0); // nothing more to do: end the whole client
}

int main() {
    int cid = 0;
    conn[cid].fd = xsocket(SOCK_STREAM);
    xconnect(conn[cid].fd, "127.0.0.1", 9999); // server's IP address and port
    printf("Connected\n");

    pthread_create(&conn[cid].thread_id, NULL, mythread, (void*)(intptr_t)cid);
    pthread_detach(conn[cid].thread_id);

    processmain(cid);

    close(conn[cid].fd);
    return 0;
}
