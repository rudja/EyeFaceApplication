///////////////////////////////////////////////////////////
//                                                       //
// Copyright (c) 2014-2016 by Eyedea Recognition, s.r.o. //
//                  ALL RIGHTS RESERVED.                 //
//                                                       //
// Author: Eyedea Recognition, s.r.o.                    //
//                                                       //
// Contact:                                              //
//           web: http://www.eyedea.cz                   //
//           email: info@eyedea.cz                       //
//                                                       //
// Consult your license regarding permissions and        //
// restrictions.                                         //
//                                                       //
///////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////
//                                                       //
// EyeFace SDK: example-logserver.cpp                    //
// ----------------------------------------------------- //
//                                                       //
// This example demonstrates a simple tcp server, which  //
// listens to EyeFace SDK's Server Logging. Run this     //
// server example first. Then configure your EyeFace SDK //
// application to use Server Log feature and setup       //
// address and port of this server via "config.ini".     //
// Finally, run your EyeFace SDK application and you     //
// will start receiving messages.                        //
//                                                       //
// Specifically, set config.ini as                       //
//                                                       //
// [LOG PARAMETERS]                                      //
// log_server_use     = 1                                //
// log_server_address = "127.0.0.1"                      //
// log_server_port    = 10000                            //
//                                                       //
// to connect your EyeFace SDK application to this       //
// server. Don't forget that the last line of config.ini //
// must be an empty line.                                //
//                                                       //
///////////////////////////////////////////////////////////

// Headers that define the standard C++ interfaces.
#include <iostream>
#include <cstring>
#include <string>

#if defined(WIN32) || defined(_WIN32) || defined(_DLL) || defined(_WINDOWS_) || defined(_WINDLL)
    // Windows
    #include <winsock2.h>
    #include <ws2tcpip.h>
#else
    // Linux
    #include <unistd.h>
    #include <sys/types.h>
    #include <sys/socket.h>
    #include <netinet/in.h>

    #define SOCKET int
#endif

#define MAX_MSG_LENGTH 8192

// Port where this example will listen. If you have another server on this port, change
// this PORT_NUMBER and also "log_server_port" in EyeFace config.ini.
#define PORT_NUMBER 10000

// This function will run during execution.
int main (int argc, char * argv[])
{
    // Server socket.
    SOCKET sockfd = 0;
    
    // Incoming connection's data.
    char buffer[MAX_MSG_LENGTH];
    
    // Windows socket startup.
#if defined(WIN32) || defined(_WIN32) || defined(_DLL) || defined(_WINDOWS_) || defined(_WINDLL)
     WSADATA wsaData;
     WSAStartup(MAKEWORD(2,2), &wsaData);
#endif
    
    // Server address.
    struct sockaddr_in server_address;
    
    // Client address.
    struct sockaddr_in client_address;
    
    // Create socket.
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    
    // Allows immediate restart of this example.
    int option = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (char*) &option, sizeof(option));
    
    if (sockfd < 0)
    {
        std::cerr << "Error creating socket!" << std::endl;
        return -1;
    }
    
    // Set server address and port.
    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(PORT_NUMBER);
    
    // Bind socket.
    if (bind(sockfd, (struct sockaddr *) &server_address, sizeof(server_address)) < 0) 
    {
        std::cerr << "Error binding socket!" << std::endl;
        return -1;
    }
    
    // Start listening.
    listen(sockfd, 5);
    
    // Prepare arrays of sockets to watch.
    timeval sel_timeout;
    fd_set read_sockets;
    fd_set changed_sockets;
    
    FD_ZERO(&read_sockets);
    FD_ZERO(&changed_sockets);
    
    // Add server main socket sockfd to watch.
    FD_SET(sockfd, &read_sockets);
    
    // Write info into console.
    std::cout << "----------------------------------" << std::endl;
    std::cout << "|                                |" << std::endl;
    std::cout << "|   EyeFace Test Server          |" << std::endl;
    std::cout << "|                                |" << std::endl;
    std::cout << "----------------------------------" << std::endl;
    
    // Listen in infinite loop.
    SOCKET max_sock_num = sockfd;
    
    while (1)
    {
        // 1 hour timeout for select(), needs to be reset every time!
        sel_timeout.tv_sec = 3600;
        sel_timeout.tv_usec = 0;
        
        // All sockets that you wish to read are copied for select.
        changed_sockets = read_sockets;
        
        int retval = select((int)max_sock_num+1, &changed_sockets, NULL, NULL, &sel_timeout); 
        
        if (retval == -1)
        {
            std::cerr << "Select failed!" << std::endl;
            return -1;
        }
        
        // Now the data are available on changed sockets. 
        // REMARK :Unfortunatelly fd_set iteration is highly system dependant.
        // This is a simplified crossplatform solution not suitable for production.
        int num_processed_sockets = 0;
        
        for (int i = 0; i <= max_sock_num; i++)
        {
            // Check if the ith socket was changed.
            if (num_processed_sockets == retval)
            {
                break;
            }
            
            if (!FD_ISSET(i, &changed_sockets))
            {
                continue;
            }
            
            num_processed_sockets++;
            
            // If there is a connection to server, accept and create new client connection.
            if(i == sockfd)
            {
                SOCKET new_client_sockfd = 0;
                socklen_t client_length = sizeof(client_address);
                new_client_sockfd = accept(sockfd, (struct sockaddr *) &client_address, &client_length);
                
                if (new_client_sockfd < 0)
                {
                    std::cerr << "Error accepting socket!" << std::endl;
                    return -1;
                }
                
                std::cout << "CONNECTION ESTABLISHED WITH CLIENT, SOCKFD = " << new_client_sockfd << "." << std::endl;
                FD_SET(new_client_sockfd, &read_sockets);
                
                if (new_client_sockfd > max_sock_num)
                {
                    max_sock_num = new_client_sockfd;
                }
            }
            else
            {
                int client_sockfd = i;
                
                std::cout << "READING CLIENT DATA, SOCK i = " << client_sockfd << "." << std::endl;
                
                // Read data from client socket.
                memset(buffer, 0, MAX_MSG_LENGTH);
                
                int n;
#if defined(WIN32) || defined(_WIN32) || defined(_DLL) || defined(_WINDOWS_) || defined(_WINDLL)
                // Windows
                n = recv(client_sockfd, buffer, MAX_MSG_LENGTH-1, NULL);
#else
                // Linux
                n = read(client_sockfd, buffer, MAX_MSG_LENGTH-1);
#endif
                // Error reading.
                if (n < 0)
                {
                    std::cout << "Error reading from socket!\n" << std::endl;
                    return -1;
                }
                
                // New message.
                if (n > 0)
                {
                    // Print received data.
                    std::cout << "Received data in raw form:" << std::endl << buffer << std::endl << std::endl;
                }
                
                // Write response. The EyeFace SDK client is waiting for it to confirm the message was accepted on our end.
                //char std_response[] = "HTTP/1.1 200 OK\r\nContent-Length: 2\r\nContent-Type=text/plain\r\n\r\nOK";
				char std_response[] = "HTTP/1.1 200 OK\r\nContent-Length: 14\r\nContent-Type=text/plain\r\n\r\nWESH MA GUEULE!";
                
#if defined(WIN32) || defined(_WIN32) || defined(_DLL) || defined(_WINDOWS_) || defined(_WINDLL)
                // Windows
                int m = send(client_sockfd, std_response, sizeof(std_response), NULL);
#else
                // Linux
                int m = write(client_sockfd, std_response, sizeof(std_response));
#endif
                if (m < 0)
                {
                    std::cerr << "Error writing to socket!" << std::endl;
                    return -1;
                }

                // Termination.
                if (n == 0)
                {
                    // Connection terminated, remove client socket.
                    FD_CLR(client_sockfd, &read_sockets);
                    
                    // Close connection to client.
#if defined(WIN32) || defined(_WIN32) || defined(_DLL) || defined(_WINDOWS_) || defined(_WINDLL)
                    // Windows
                    closesocket(client_sockfd);
#else
                    // Linux
                    close(client_sockfd);
#endif
                    std::cout << "CLIENT CONNECTION TERMINATED, SOCK: " << client_sockfd << "." << std::endl;
                }
            }
        }
    }
    
#if defined(WIN32) || defined(_WIN32) || defined(_DLL) || defined(_WINDOWS_) || defined(_WINDLL)
    // Windows
    closesocket(sockfd);
    WSACleanup();
#else
    // Linux
    close(sockfd);
#endif
    
    return 0;
}
