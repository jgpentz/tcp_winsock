#include <stdio.h>
#include <stdint.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27015"

int main(int argc, char **argv)
{
    int32_t i_result;
    WSADATA wsaData;

    int32_t recvbuflen = DEFAULT_BUFLEN;

    uint8_t *sendbuf = "this is a test";
    char recvbuf[DEFAULT_BUFLEN];

    // socket objects
    struct addrinfo *result = NULL;
    struct addrinfo *ptr = NULL;
    struct addrinfo hints;
    SOCKET ConnectSocket = INVALID_SOCKET;

    // Validate the parameters
    if (argc != 2) {
        printf("usage: %s server-name\n", argv[0]);
        return 1;
    }

    // Initialize Winsock
    i_result = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (i_result != 0) {
        printf("WSAStartup failed: %d\n", i_result);
        return 1;
    }

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    // Resolve the server address and port
    i_result = getaddrinfo(argv[1], DEFAULT_PORT, &hints, &result);
    if (i_result != 0) {
        printf("getaddrinfo failed: %d\n", i_result);
        WSACleanup();
        return 1;
    }

    // Attempt to connect to the first address returned by
    // the call to getaddrinfo
    ptr=result;

    // Create a SOCKET for connecting to server
    ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);

    // Check to make sure that a valid socket was created
    if (ConnectSocket == INVALID_SOCKET) {
        printf("Error at socket(): %ld\n", WSAGetLastError());
        freeaddrinfo(result);
        WSACleanup();
        return 1;
    }

    // Connect to server.
    i_result = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
    if (i_result == SOCKET_ERROR) {
        closesocket(ConnectSocket);
        ConnectSocket = INVALID_SOCKET;
    }

    // Should really try the next address returned by getaddrinfo
    // if the connect call failed
    // But for this simple example we just free the resources
    // returned by getaddrinfo and print an error message

    freeaddrinfo(result);

    if (ConnectSocket == INVALID_SOCKET) {
        printf("Unable to connect to server!\n");
        WSACleanup();
        return 1;
    }

    // Send an initial buffer
    i_result = send(ConnectSocket, sendbuf, (int)strlen(sendbuf), 0 );
    if (i_result == SOCKET_ERROR) {
        printf("send failed with error: %d\n", WSAGetLastError());
        closesocket(ConnectSocket);
        WSACleanup();
        return 1;
    }

    printf("Bytes Sent: %ld\n", i_result);

    // shutdown the connection since no more data will be sent
    i_result = shutdown(ConnectSocket, SD_SEND);
    if (i_result == SOCKET_ERROR) {
        printf("shutdown failed with error: %d\n", WSAGetLastError());
        closesocket(ConnectSocket);
        WSACleanup();
        return 1;
    }

    // Receive until the peer closes the connection
    do {

        i_result = recv(ConnectSocket, recvbuf, recvbuflen, 0);
        if ( i_result > 0 )
            printf("Bytes received: %d\n", i_result);
        else if ( i_result == 0 )
            printf("Connection closed\n");
        else
            printf("recv failed with error: %d\n", WSAGetLastError());

    } while( i_result > 0 );

    // cleanup
    closesocket(ConnectSocket);
    WSACleanup();

    return 0;
}