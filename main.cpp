#include "http_tcp_server.h"

int main() {
    ben::TcpServer server = ben::TcpServer("0.0.0.0", 8080);
    server.StartListening();
    return 0;
}
