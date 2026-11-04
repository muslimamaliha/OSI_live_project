
#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
typedef SOCKET socket_t;
#define CLOSESOCKET closesocket
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
typedef int socket_t;
#define INVALID_SOCKET (-1)
#define SOCKET_ERROR (-1)
#define CLOSESOCKET close
#endif

#include "common.h"

int main(int argc, char *argv[])
{
    const char *host = (argc >= 2) ? argv[1] : "127.0.0.1";
    const char *message = (argc >= 3) ? argv[2] : "Hello from OSI Client";

    if (strlen(message) > 3000)
    {
        fprintf(stderr, "Message too long. Please use 3000 characters or fewer.\n");
        return 1;
    }

#ifdef _WIN32
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
    {
        fprintf(stderr, "WSAStartup failed\n");
        return 1;
    }
#endif

    char data[BUFFER_SIZE];
    strncpy(data, message, sizeof(data) - 1);
    data[sizeof(data) - 1] = '\0';

    printf("[CLIENT] Original message: %s\n", data);
    printf("[SENDER] Starting encapsulation: Application -> Physical\n");

    for (int i = 0; i < 7; i++)
    {
        if (!add_header(data, sizeof(data), i))
        {
            fprintf(stderr, "Could not add Layer %d header: buffer limit reached.\n", 7 - i);
#ifdef _WIN32
            WSACleanup();
#endif
            return 1;
        }

        printf("[ENCAPSULATION] Layer %d (%s): header added\n",
               7 - i, layer_names[i]);

        if ((i + 1) % 2 == 0 || i == 6)
        {
            printf("[PROGRESS] %d/7 layers completed at sender.\n", i + 1);
        }
    }

    socket_t sock = socket(AF_INET, SOCK_STREAM, 0);

    if (sock == INVALID_SOCKET)
    {
        fprintf(stderr, "Could not create client socket\n");
#ifdef _WIN32
        WSACleanup();
#endif
        return 1;
    }

    struct sockaddr_in server;
    memset(&server, 0, sizeof(server));

    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);

    /* Use inet_addr instead of inet_pton for older MinGW GCC */
    server.sin_addr.s_addr = inet_addr(host);

    if (server.sin_addr.s_addr == INADDR_NONE)
    {
        fprintf(stderr, "Invalid server IP address: %s\n", host);
        CLOSESOCKET(sock);
#ifdef _WIN32
        WSACleanup();
#endif
        return 1;
    }

    printf("[NETWORK] Connecting to %s:%d...\n", host, PORT);

    if (connect(sock, (struct sockaddr *)&server, sizeof(server)) == SOCKET_ERROR)
    {
        fprintf(stderr, "Connection failed. Start the server first.\n");
        CLOSESOCKET(sock);
#ifdef _WIN32
        WSACleanup();
#endif
        return 1;
    }

    int sent = send(sock, data, (int)strlen(data), 0);

    if (sent == SOCKET_ERROR)
    {
        fprintf(stderr, "Send failed\n");
        CLOSESOCKET(sock);
#ifdef _WIN32
        WSACleanup();
#endif
        return 1;
    }

    printf("[NETWORK] Sent %d bytes through a real TCP socket.\n", sent);

    char ack[256] = {0};
    int n = recv(sock, ack, sizeof(ack) - 1, 0);

    if (n > 0)
    {
        ack[n] = '\0';
        printf("[CLIENT] Server response: %s\n", ack);
    }

    CLOSESOCKET(sock);

#ifdef _WIN32
    WSACleanup();
#endif

    printf("[CLIENT] Transmission finished.\n");

    return 0;
}
