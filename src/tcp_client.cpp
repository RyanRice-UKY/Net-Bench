#include <iostream>
#include <cstdint>
#include <cstdio>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

using namespace std;

int main()
{
    //---------------Create Client Socket--------------
    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);

    if (clientSocket == -1)
    {
        perror("socket");
        return 1;
    }

    int portNum = 8080;

    //-------------Target Server Address---------------
    sockaddr_in serverAddress{};

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(portNum);

    if (inet_pton(AF_INET,"127.0.0.1",&serverAddress.sin_addr) != 1)//inet_pton makes serverAddress 127.0.0.1
    {
        cerr << "Failed to convert server IP address\n";
        close(clientSocket);
        return 1;
    }

    //-------------Connect Client To Server---------------
    if (connect(clientSocket,reinterpret_cast<sockaddr*>(&serverAddress),sizeof(serverAddress)) == -1)
    {
        perror("connect");
        close(clientSocket);
        return 1;
    }

    //-------------Send integers and receive echo---------------
    for (uint32_t i = 1; i <= 1000; ++i)
    {
        uint32_t networkNumber = htonl(i);

        ssize_t bytesSent = send(clientSocket,&networkNumber,sizeof(networkNumber),0);

        if (bytesSent == -1)
        {
            perror("send");
            close(clientSocket);
            return 1;
        }

        uint32_t receivedNetworkNumber{};

        ssize_t bytesReceived = recv(clientSocket,&receivedNetworkNumber,sizeof(receivedNetworkNumber),MSG_WAITALL);

        if (bytesReceived == -1)
        {
            perror("recv");
            close(clientSocket);
            return 1;
        }

        if (bytesReceived == 0)
        {
            cout << "Server closed connection\n";
            break;
        }

        uint32_t receivedNumber = ntohl(receivedNetworkNumber);

        cout << "Sent: " << i
             << " Received: " << receivedNumber
             << '\n';
    }

    close(clientSocket);

    return 0;
}
