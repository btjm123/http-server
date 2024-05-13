//
// Created by Benn Tan on 13/5/24.
//

// this prevents the header files from being preprocessed and included twice
#ifndef HTTP_TCP_SERVER_H
#define HTTP_TCP_SERVER_H

#include <string>
#include <netinet/in.h>

namespace ben {
    class TcpServer {
    public:
        TcpServer(std::string ip_addr, int port);
        ~TcpServer();
        int StartServer();
        void CloseServer();
        void StartListening();
        void AcceptConnection();
        void SendResponse();
    private:
        int sfd;
        int cfd;
        int port;
        struct sockaddr_in socketAddress;
        socklen_t socketAddressLen;
    };
} // namespace ben

#endif //HTTP_TCP_SERVER_H
