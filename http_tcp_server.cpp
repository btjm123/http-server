//
// Created by Benn Tan on 13/5/24.
//

#include "http_tcp_server.h"

#include <iostream>
#include <string>
#include <valarray>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sstream>
#include <unistd.h>

namespace {

    const int MAX_BUFFER_SIZE = 1024;
    const int MAX_CONNECTIONS = 20;
    void log(const std::string& msg) {
        std::cout << msg << std::endl;
    }
    void exitWithError(const std::string& err) {
        log(err);
        exit(1);
    }
}

namespace ben {
    TcpServer::TcpServer(std::string ip_addr, int port): port(port), sfd(), socketAddress(), socketAddressLen(), cfd() {
        socketAddress.sin_family = AF_INET;
        socketAddress.sin_port = htons(port);
        socketAddress.sin_addr.s_addr = inet_addr(ip_addr.c_str());
        socketAddressLen = sizeof(socketAddress);

        if (StartServer() != 0) {
            std::ostringstream ss;
            ss << "Server failed to start with IP Address:  " << inet_ntoa(socketAddress.sin_addr) << " and PORT: " << ntohs(socketAddress.sin_port);
            log(ss.str());
            return;
        }

        std::ostringstream ss;
        ss << "Server has started with IP Address:  " << inet_ntoa(socketAddress.sin_addr) << " and PORT: " << ntohs(socketAddress.sin_port);
        log(ss.str());
    }

    TcpServer::~TcpServer() {
        CloseServer();
    }


    int TcpServer::StartServer() {
        // AFINET = IPv4 Addresses | SOCK_STREAM = full-duplex byte streams
        // Must be in a connected state before invoking read / write calls
        sfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sfd < 0) {
            exitWithError("Creating socket failed");
            return 1;
        }

        // Right after disconnecting, the socket might be in a TIME_WAIT state - during which we might be unable to bind to the same IP address
        // This option enables the sfd to reuse the same IP Address (so that we don't have to wait after the kernel destroys the sfd i tink)
        int reuse = 1;
        if (setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, (void*) &reuse, sizeof(reuse)) < 0) {
            exitWithError("Failed to set socket options for some reason");
            return 1;
        }

        if (bind(sfd, (struct sockaddr*) &socketAddress, socketAddressLen) < 0) {
            exitWithError("Binding socket failed");
            return 1;
        }

        return 0;
    }

    void TcpServer::StartListening() {
        // This marks the sfd as a passive socket; ready to receiving incoming connections
        // The second parameter denotes the max no. of connections in the queue
        if (listen(sfd, MAX_CONNECTIONS) < 0) {
            std::ostringstream ss;
            ss << "Error encountered when server tried to listen on IP Address:" << inet_ntoa(socketAddress.sin_addr) << " and PORT: " << ntohs(socketAddress.sin_port);
            exitWithError(ss.str());
        }

        std::ostringstream ss;
        ss << "Started listening on IP Address:" << inet_ntoa(socketAddress.sin_addr) << " and PORT: "  << ntohs(socketAddress.sin_port);
        log(ss.str());

        while (1) {
            // Let's accept the connection first
            AcceptConnection();

            // Attempt to read from the client file descriptor
            // We assume that the bytes received from the client fd < MAX_BUFFER_SIZE
            char buffer[MAX_BUFFER_SIZE] = {0};
            int bytes = read(cfd, buffer, MAX_BUFFER_SIZE);
            if (bytes < 0) {
                std::ostringstream ss;
                ss << "Error encountered when trying to read";
                exitWithError(ss.str());
            }

            // It's better to scope it out (so that streams can be disposed when no longer needed) as opposed to flushing and reusing an existing stream
            // https://stackoverflow.com/questions/11458980/how-do-you-clear-a-stdostringstream-instance-so-it-can-be-reused
            {
                std::ostringstream ss;
                ss << "Read from client successfully";
                log(ss.str());
            }

            SendResponse();
            close(cfd);

        }
    }

    void TcpServer::AcceptConnection() {
        cfd = accept(sfd, (struct sockaddr *) &socketAddress, (socklen_t *) &socketAddressLen);
        if (cfd < 0) {
            std::ostringstream ss;
            ss << "Error encountered when trying to accept client connection";
            exitWithError(ss.str());
        }
    }

    void TcpServer::SendResponse() {
        std::string response = "HTTP/1.1 200 OK\r\n\r\n";
        long bytesSent = write(cfd, response.c_str(), response.size());
        if (bytesSent == response.size()) {
            std::ostringstream ss;
            ss << "Wrote to client successfully!";
            log(ss.str());
        } else {
            std::ostringstream ss;
            ss << "Error writing to client";
            exitWithError(ss.str());
        }
    }


    void TcpServer::CloseServer() {
        close(sfd);
        close(cfd);
    }

}// namespace ben