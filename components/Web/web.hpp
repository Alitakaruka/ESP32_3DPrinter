#pragma once
#include "lwip/sockets.h"
#include "lwip/netdb.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include "esp_netif_ip_addr.h" // ip4addr_ntoa
#include "esp_netif_types.h"
#include "esp_netif.h"

namespace net
{
    class Conn
    {
    private:
        int clientDescriptor = -1;

    public:
        Conn(){};
        Conn(int clientDes) { this->clientDescriptor = clientDes; }
        Conn (const Conn& other): clientDescriptor(other.clientDescriptor){}
        bool isValidConnection(){return this->clientDescriptor >= 0;}
        int Write(const void *buffer, int bufferLen = -1);
        int Read(const void *buffer, int bufferLen);
        void Close();
        bool IsClosed();
    };

    class Listener
    {
    private:
        int Fd = -1;
    public:
        Listener(int fD) { this->Fd = fD; }
        ~Listener(){if (this->Fd >= 0){close(Fd);}}
          Conn AcceptConn(){
                struct sockaddr_in client_addr;
                socklen_t socklen = sizeof(client_addr);
                int client = accept(Fd, (struct sockaddr *)&client_addr, &socklen);
                Conn conn(client);
                return conn;
          }
          Conn AcceptConn_Async(){
               struct sockaddr_in client_addr;
                socklen_t socklen = sizeof(client_addr);
                int client = accept(Fd, (struct sockaddr *)&client_addr, &socklen);
                Conn conn(client);
                return conn;
          }
    };

    static Listener ListenPort(const char *network, int port)
    {
        int server_fd = socket(AF_INET, SOCK_STREAM, 0);
        //fcntl(server_fd, F_SETFL, O_NONBLOCK); //NonBlock
        int opt = 1;
        setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

        struct sockaddr_in addr;
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = INADDR_ANY;
        addr.sin_port = htons(port);

        bind(server_fd, (struct sockaddr *)&addr, sizeof(addr));
        listen(server_fd, 5);
        Listener ls(server_fd);
        return ls;
    }


}
