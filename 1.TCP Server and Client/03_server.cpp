#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/ip.h>

static void msg(const char *msg){
    fprintf(stderr, "%s\n", msg);
}

static void die(const char *msg){
    int err = errno;
    fprintf(stderr, "[%d] %s\n", err, msg);
    abort();
}

static void do_something(int connfd){
    // Step 1: Read a message from the client
    char rbuf[64] = {};
    ssize_t n = read(connfd, rbuf, sizeof(rbuf) - 1);
    if(n < 0){
        msg("read() error");
        return;
    }
    fprintf(stderr, "client says : %s\n", rbuf);

    // Step 2: Write a response to the client
    char msg[] = "world";
    write(connfd, msg, strlen(msg));
}

int main(){
    // Step 1: Obtain a socket handle
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if(fd < 0){
        die("socket()");
    }

    // Allow the socket to be reused immediately after the program exits
    // this is needed for most server applications
    // step 2: set socket options
    int val = 1;
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val));

    // Step 3: Bind the socket to a port {wildcard address 0.0.0.0, port 1234}
    struct sockaddr_in addr = {};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(1234);
    addr.sin_addr.s_addr = htonl(0); // wildcard address 0.0.0.0

    int rv = bind(fd, (const struct sockaddr *)&addr, sizeof(addr));
    if(rv < 0){
        die("bind()");
    }

    // step 4: Listen for incoming connections
    rv = listen(fd, SOMAXCONN);
    if(rv){
        die("listen()");
    }

    // Step 5 & 6: Accept connections and Read/Write
    while(true){
        struct sockaddr_in client_addr = {};
        socklen_t client_addr_len = sizeof(client_addr);
        int connfd = accept(fd, (struct sockaddr *)&client_addr, &client_addr_len);
        if(connfd < 0){
            msg("accept() error");
            continue;
        }
        do_something(connfd);
        close(connfd);
    }
    return 0;
}
