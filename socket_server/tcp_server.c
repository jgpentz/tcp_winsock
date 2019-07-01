#include <stdio.h>
#include <stdint.h>
#include <winsock2.h>
#include <ws2tcpip.h>

int main()
{
    int32_t i_result;
    WSADATA wsaData;

// Initialize Winsock
i_result = WSAStartup(MAKEWORD(2,2), &wsaData);
if (i_result != 0) {
    printf("WSAStartup failed: %d\n", i_result);
    return 1;
}

    printf("Hello World\n");
}