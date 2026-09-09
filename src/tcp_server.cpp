#include <iostream>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>

using namespace std;

int main()
{
                //First Step of building a tcp server is the server socket
//-----------------------------------------------------------------------------
    //initialize socket object and link serverSocket to the file descriptor int
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);

    int portNum = 8080;//input into htons to correct the byte ordering (little/big endian)

    // The{}after address prevents garbage bits to filling undefined field inside this struct 
    sockaddr_in address{};
    address.sin_family = AF_INET;//AF_INET = IPv4 format
    address.sin_port = htons(portNum);// port
    address.sin_addr.s_addr = htonl(INADDR_ANY);//since address is our socket address we must use
    //s_addr. s_addr is a IPv4 address since it is useful on its own we must call the object it is
    //defined in which is in_addr.s_addr
    //INADDR_ANY = accept traffic from any IPv4

                //bind the socket IP address to the server socket
//------------------------------------------------------------------------------
    //now we must bind our created socket address and our server socket together
    if(bind(serverSocket, reinterpret_cast<sockaddr*>(&address) ,sizeof(address)) == -1){
        cerr << "Failed to bind socket\n";
        close(serverSocket);
        return 1;
    }

                //listen
//-------------------------------------------------------------------------------
    if(listen(serverSocket, SOMAXCONN) == -1){//SOMAXCONN defines the backlog/number waiting for accept
        cerr << "Failed to listen on socket\n";
        close(serverSocket);
        return 1;
    }
    
                //Accept packet
//---------------------------------------------------------------------------------
    int clientSocket = accept(serverSocket, nullptr, nullptr);//nullptr since knowing the clients Ip
    //is not important yet

    if(clientSocket == -1){
        cerr << "Failed to accept\n";
        close(serverSocket);
        return 1;
    }

    //----------- Loop until client is don sending packets ----------------
    for(;;){
    //------------------------------- recieve ---------------------------------------------
        //create recieve buffer
        char buffer[4096];
        ssize_t byteCount = recv(clientSocket, buffer, sizeof(buffer), 0);
    
        if(byteCount == -1){
            cerr << "Failed to recieve";
            close(clientSocket);
            close(serverSocket);
            return 1;
        }
        else if(byteCount == 0){
            break;//connection closed
        }

            
    //----------------------------- send -------------------------------------------------
        ssize_t totalSent = 0;
        
        while(totalSent < byteCount){

            ssize_t bytesSent = send(clientSocket, buffer, byteCount, 0);

            if (bytesSent == -1){
                cerr << "Failed to send\n";
                close(clientSocket);
                close(serverSocket);
                return 1;
            }
            totalSent += bytesSent;
            
        }
    }
    close(clientSocket);
    close(serverSocket);



    return 0;
}
