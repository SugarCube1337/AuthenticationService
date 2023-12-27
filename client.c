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
    ssize_t n;
    struct sockaddr_in saddr;
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(PORT);
    saddr.sin_addr.s_addr = inet_addr(serverAddr);

    char recBuf[4096];

    printf("Send to server: %s\n", msg);

    for(i = 0; i < MAX_TRY_SOCK_CONNECT; i++) {
        printf("Try: %d\n", i);

        int sock = CreateSocket();
        if (sock == -1) {
            return 0;
        }

        int isConnected = connect(sock, (const struct sockaddr *) &saddr, sizeof(saddr));
        if(isConnected == -1) {
            printf("connect() ret %d, %s\n", errno, strerror(errno));
            usleep(DELAY_SOCK_CONNECT);
            continue;
        }

        /* send request */
        size_t total = strlen(msg);
        size_t sent = 0;
        do {
            n = write(sock, msg + sent, total-sent);
            if (n < 1) // -1 or 0
                break;
            sent += n;
        } while (sent < total);
        if (n == -1) {
            printf("write() ret %d %s\n", errno, strerror(errno));
            close(sock);
            usleep(DELAY_SOCK_CONNECT);
            continue;
        }

        /* receive response */
        memset(recBuf, 0, sizeof(recBuf));
        total = sizeof(recBuf)-1;
        size_t received = 0;
        do {
            n = read(sock, recBuf+received, total-received);
            if (n < 1)
                break; // -1 or 0
            received += n;
        } while (received < total);
        if (n == -1) {
            printf("read() ret %d %s\n", errno, strerror(errno));
            close(sock);
            usleep(DELAY_SOCK_CONNECT);
            continue;
        }
        if (received == total)
            printf("error storing full response from server\n");

        printf("Got from server:\n %s", recBuf);
        close(sock);
        break;
    }

    if (i == MAX_TRY_SOCK_CONNECT)
        return 0;
    return 1;
}

int main(int argc, char *argv[]) {
    char servAddr[] = "127.0.0.1";
    AskServer(servAddr, "GET /token?name=pupsik HTTP/1.0\r\n\r\n");
    AskServer(servAddr, "POST /validate HTTP/1.0\r\n"
                        "Host: somebackend\r\n"
                        "Content-Type: application/json\r\n"
                        "Content-Length: 17\r\n"
                        "\r\n"
                        "{\"name\": \"pusik\"}\r\n"
                        "\r\n");
    AskServer(servAddr, "exit");
    return 0;
}