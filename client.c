#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 7777
#define MAX_TRY_SOCK_CONNECT 3
#define DELAY_SOCK_CONNECT 3 * 100000

int CreateSocket() {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if(sock == -1) {
        printf("socket() ret %d, %s\n", errno, strerror(errno));
        return -1;
    }
    return sock;
}

int AskServer(const char *serverAddr, const char *msg) {
    int i;
    struct sockaddr_in saddr;
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(PORT);
    saddr.sin_addr.s_addr = inet_addr(serverAddr);

    char sendBuf[1024];
    memset(sendBuf, 0, sizeof(sendBuf));

    char recBuf[1024 + 16];
    memset(recBuf, 0, sizeof(recBuf));

    printf("Send to server: %s\n", msg);
    strcpy(sendBuf, msg);

    for(i = 0; i < MAX_TRY_SOCK_CONNECT; i++) {
        printf("Try: %d\n", i);

        int sock = CreateSocket();

        int isConnected = connect(sock, (const struct sockaddr *) &saddr, sizeof(saddr));
        if(isConnected == -1) {
            printf("connect() ret %d, %s\n", errno, strerror(errno));
            usleep(DELAY_SOCK_CONNECT);
            continue;
        }

        ssize_t n = write(sock, sendBuf, strlen(sendBuf));
        if (n == -1) {
            printf("write() ret %d %s\n", errno, strerror(errno));
            close(sock);
            usleep(DELAY_SOCK_CONNECT);
            continue;
        }

        n = read(sock, recBuf, sizeof(recBuf));
        if(n == -1) {
            printf("read() ret %d %s\n", errno, strerror(errno));
            close(sock);
            usleep(DELAY_SOCK_CONNECT);
            continue;
        }
        printf("Got from server: %s\n", recBuf);
        close(sock);
        break;
    }

    if (i == MAX_TRY_SOCK_CONNECT)
        return 0;
    return 1;
}

int main(int argc, char *argv[]) {
    char servAddr[] = "127.0.0.1";
    AskServer(servAddr, "hello from client");
    AskServer(servAddr, "exit");
    return 0;
}