#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/ip.h>

static void die(const char *msg){
    int err = errno;
    fprintf(stderr, "[%d] %s\n", err, msg);
    abort();
}

int main(){
    // Step 1: Obtain a socket handle
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if(fd < 0){
        die("socket()");
    }

    // Step 2: Connect to the server
    struct sockaddr_in addr = {};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(1234);
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK); // 127.0.0.1

    int rv = connect(fd, (struct sockaddr *)&addr, sizeof(addr));
    if(rv < 0){
        die("connect()");
    }

    // Step 3: Write a message to the server
    char msg[] = "hello";
    write(fd, msg, strlen(msg));

    // Step 4: Read the response
    char rbuf[64] = {};
    ssize_t n = read(fd, rbuf, sizeof(rbuf) - 1);
    if(n < 0){
        die("read()");
    }
    printf("server says : %s\n", rbuf);

    // step 5: Close the socket
    close(fd);

    return 0;
}