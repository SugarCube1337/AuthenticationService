#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

#define PORT 7777
#define MAX_TRY_SOCK_CONNECT 3
#define DELAY_SOCK_CONNECT 3 * 100000

void InitOpenSslLib() {
    SSL_library_init();
    SSL_load_error_strings();
}
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

    SSL_CTX *ctx;
    SSL *sslConn;

    struct sockaddr_in saddr;
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(PORT);
    saddr.sin_addr.s_addr = inet_addr(serverAddr);

    char recBuf[4096];

    printf("Send to server: %s\n", msg);

    for(i = 0; i < MAX_TRY_SOCK_CONNECT; i++) {
        printf("Try: %d\n", i);

        /* Create a TLS client context with a CA certificate */
        ctx = SSL_CTX_new(TLS_client_method());
        SSL_CTX_use_certificate_file(ctx, "../deploy/ca-cert.pem", SSL_FILETYPE_PEM);

        int sock = CreateSocket();
        if (sock == -1) {
            return 0;
        }

        sslConn = SSL_new(ctx);
        SSL_set_fd(sslConn, sock); // create ssl session and bind it with socket

        int isConnected = connect(sock, (const struct sockaddr *) &saddr, sizeof(saddr));
        if(isConnected == -1) {
            printf("connect() ret %d, %s\n", errno, strerror(errno));
            usleep(DELAY_SOCK_CONNECT);
            continue;
        }

        // run ssl handshake
        if (SSL_connect(sslConn)) {

            /* send request */
            size_t total = strlen(msg);
            size_t sent = 0;
            do {
                n = SSL_write(sslConn, msg + sent, total-sent);
                if (n < 1) // -1 or 0
                    break;
                sent += n;
            } while (sent < total);
            if (n == -1) {
                ERR_print_errors_fp(stderr);
                close(sock);
                SSL_free(sslConn);
                SSL_CTX_free(ctx);
                usleep(DELAY_SOCK_CONNECT);
                continue;
            }

            /* receive response */
            memset(recBuf, 0, sizeof(recBuf));
            total = sizeof(recBuf)-1;
            size_t received = 0;
            do {
                n = SSL_read(sslConn, recBuf+received, total-received);
                if (n < 1)
                    break; // -1 or 0
                received += n;
            } while (received < total);
            if (n == -1) {
                ERR_print_errors_fp(stderr);
                close(sock);
                SSL_free(sslConn);
                SSL_CTX_free(ctx);
                usleep(DELAY_SOCK_CONNECT);
                continue;
            }
            if (received == total)
                printf("error storing full response from server\n");

            printf("Got from server:\n %s", recBuf);
        }

        SSL_free(sslConn);
        SSL_CTX_free(ctx);
        close(sock);
        break;
    }

    if (i == MAX_TRY_SOCK_CONNECT)
        return 0;
    return 1;
}

int main(int argc, char *argv[]) {
    InitOpenSslLib();

    char servAddr[] = "127.0.0.1";
    AskServer(servAddr, "GET /token?name=pupsik HTTP/1.0\r\n\r\n"); // in norm app query_string should contain other user's identification, for example hash from pass
    printf("\n-------------------------\n");
    AskServer(servAddr, "POST /validate HTTP/1.0\r\n"
                        "Host: somebackend\r\n"
                        "Content-Type: plain/text\r\n"
                        "Content-Length: 186\r\n"
                        "\r\n"
                        "eyJhbGciOiAiSFMyNTYiLCAidHlwIjogIkpXVCJ9.eyJfaWQiOiAiMjc3MzUzZGVhZCIsICJ1c2VybmFtZSI6ICJwdXBzaWsiLCAic2VydmljZXMiOiBbInNlcnYxIiwgInNlcnYyIl19.0u12/m86MXmmiaT14q4w9YF8UM70AqFHOVSzq994uns=\r\n\r\n");
    printf("\n-------------------------\n");
    AskServer(servAddr, "exit");
    return 0;
}