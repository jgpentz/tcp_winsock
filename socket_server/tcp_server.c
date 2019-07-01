#include <stdio.h>
#include <stdint.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27015"

int main(void)
{
    int32_t i_result;
    WSADATA wsaData;

    struct addrinfo *result = NULL;
    struct addrinfo *ptr = NULL;
    struct addrinfo hints;

    SOCKET ListenSocket = INVALID_SOCKET;
    SOCKET ClientSocket = INVALID_SOCKET;

    int32_t i_send_result;
    char recvbuf[DEFAULT_BUFLEN];
    int32_t recvbuflen = DEFAULT_BUFLEN;

    // Initialize Winsock
    i_result = WSAStartup(MAKEWORD(2,2), &wsaData);
    if(i_result != 0)
    {
        printf("WSAStartup failed: %d\n", i_result);
        return 1;
    }

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    // Resolve the local address and port to be used by the server
    i_result = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
    if(i_result != 0)
    {
        printf("getaddrinfo failed: %d\n", i_result);
        WSACleanup();
        return 1;
    }

    // Create a SOCKET for the server to listen for client connections
    ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);

    if(ListenSocket == INVALID_SOCKET)
    {
        printf("Error at socket(): %ld\n", WSAGetLastError());
        freeaddrinfo(result);
        WSACleanup();
        return 1;
    }

    // Setup the TCP listening socket
    i_result = bind( ListenSocket, result->ai_addr, (int)result->ai_addrlen);
    if(i_result == SOCKET_ERROR)
    {
        printf("bind failed with error: %d\n", WSAGetLastError());
        freeaddrinfo(result);
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    freeaddrinfo(result);

    if(listen(ListenSocket, SOMAXCONN) == SOCKET_ERROR)
    {
        printf( "Listen failed with error: %ld\n", WSAGetLastError() );
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }


    ClientSocket = INVALID_SOCKET;

    // Accept a client socket
    ClientSocket = accept(ListenSocket, NULL, NULL);
    if (ClientSocket == INVALID_SOCKET)
    {
        printf("accept failed: %d\n", WSAGetLastError());
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    // No longer need server socket
    closesocket(ListenSocket);


    // Receive until the peer shuts down the connection
    do
    {

        i_result = recv(ClientSocket, recvbuf, recvbuflen, 0);
        if (i_result > 0)
        {
            printf("Bytes received: %d\n", i_result);

            // Echo the buffer back to the sender
            i_send_result = send( ClientSocket, recvbuf, i_result, 0 );
            if (i_send_result == SOCKET_ERROR) {
                printf("send failed with error: %d\n", WSAGetLastError());
                closesocket(ClientSocket);
                WSACleanup();
                return 1;
            }
            printf("Bytes sent: %d\n", i_send_result);
        }
        else if (i_result == 0)
            printf("Connection closing...\n");
        else
        {
            printf("recv failed with error: %d\n", WSAGetLastError());
            closesocket(ClientSocket);
            WSACleanup();
            return 1;
        }

    } while (i_result > 0);

    // shutdown the connection since we're done
    i_result = shutdown(ClientSocket, SD_SEND);
    if (i_result == SOCKET_ERROR)
    {
        printf("shutdown failed with error: %d\n", WSAGetLastError());
        closesocket(ClientSocket);
        WSACleanup();
        return 1;
    }

    // cleanup
    closesocket(ClientSocket);
    WSACleanup();

}