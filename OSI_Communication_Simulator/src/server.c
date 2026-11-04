#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "Ws2_32.lib")
typedef SOCKET socket_t;
#define CLOSESOCKET closesocket
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
typedef int socket_t;
#define INVALID_SOCKET (-1)
#define SOCKET_ERROR (-1)
#define CLOSESOCKET close
#endif

#include "common.h"

int main(void) {
#ifdef _WIN32
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2,2), &wsa) != 0) {
        fprintf(stderr, "WSAStartup failed\n"); return 1;
    }
#endif
    socket_t server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == INVALID_SOCKET) {
        fprintf(stderr, "Could not create server socket\n"); return 1;
    }

    struct sockaddr_in address;
    memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = htonl(INADDR_ANY);
    address.sin_port = htons(PORT);

    int reuse = 1;
#ifdef _WIN32
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, (const char *)&reuse, sizeof(reuse));
#else
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));
#endif

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) == SOCKET_ERROR) {
        fprintf(stderr, "Bind failed. Is port %d already in use?\n", PORT);
        CLOSESOCKET(server_fd); return 1;
    }
    if (listen(server_fd, 1) == SOCKET_ERROR) {
        fprintf(stderr, "Listen failed\n"); CLOSESOCKET(server_fd); return 1;
    }

    printf("[SERVER] Listening on port %d\n", PORT);
    fflush(stdout);

    struct sockaddr_in client_address;
#ifdef _WIN32
    int client_len = sizeof(client_address);
#else
    socklen_t client_len = sizeof(client_address);
#endif
    socket_t client_fd = accept(server_fd, (struct sockaddr *)&client_address, &client_len);
    if (client_fd == INVALID_SOCKET) {
        fprintf(stderr, "Accept failed\n"); CLOSESOCKET(server_fd); return 1;
    }

    char data[BUFFER_SIZE];
#ifdef _WIN32
    int received = recv(client_fd, data, BUFFER_SIZE - 1, 0);
#else
    ssize_t received = recv(client_fd, data, BUFFER_SIZE - 1, 0);
#endif
    if (received <= 0) {
        printf("[SERVER] No data received.\n");
    } else {
        data[received] = '\0';
        printf("[SERVER] TCP connection established with client.\n");
        printf("[SERVER] Encapsulated data received (%d bytes).\n", (int)received);
        printf("[RECEIVER] Starting decapsulation: Physical -> Application\n");

        for (int layer = 1; layer <= 7; layer++) {
            if (!remove_header(data, layer)) {
                printf("[ERROR] Invalid or missing Layer %d header. Data may be corrupted.\n", layer);
                CLOSESOCKET(client_fd); CLOSESOCKET(server_fd);
#ifdef _WIN32
                WSACleanup();
#endif
                return 1;
            }
            printf("[DECAPSULATION] Layer %d (%s): header removed\n",
                   layer, layer_names[7 - layer]);
            if (layer % 2 == 0 || layer == 7) {
                printf("[PROGRESS] %d/7 layers processed at receiver.\n", layer);
            }
        }

        printf("[SERVER] Original message: %s\n", data);
        const char *ack = "ACK: Message received successfully";
        send(client_fd, ack, (int)strlen(ack), 0);
    }

    CLOSESOCKET(client_fd);
    CLOSESOCKET(server_fd);
#ifdef _WIN32
    WSACleanup();
#endif
    printf("[SERVER] Connection closed.\n");
    return 0;
}
