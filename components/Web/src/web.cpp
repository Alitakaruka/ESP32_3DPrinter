#include "web.hpp"


int net::Conn::Write(const void* buffer,int bufferLen){
    if(bufferLen == -1){
        bufferLen = strlen((const char*)buffer);
    }
    return write(this->clientDescriptor,buffer,bufferLen);
}


int net::Conn::Read(const void* buffer,int bufferLen){
    return read(this->clientDescriptor,(void*)buffer,bufferLen);
}



void net::Conn::Close(){
    close(this->clientDescriptor);
}











// void tcp_server_task(void *pvParameters)
// {
//     int server_fd = socket(AF_INET, SOCK_STREAM, 0);
//     fcntl(server_fd, F_SETFL, O_NONBLOCK);
//     int opt = 1;
//     setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

//     struct sockaddr_in addr;
//     addr.sin_family = AF_INET;
//     addr.sin_addr.s_addr = INADDR_ANY;
//     addr.sin_port = htons(CNC_WEB_PORT);

//     bind(server_fd, (struct sockaddr *)&addr, sizeof(addr));
//     listen(server_fd, 5);

//     while (1)
//     {
//         vTaskDelay(pdMS_TO_TICKS(1000));
//         struct sockaddr_in client_addr;
//         socklen_t socklen = sizeof(client_addr);
//         int client = accept(server_fd, (struct sockaddr *)&client_addr, &socklen);
//         if (client < 0)
//         {
//             continue;
//         }
//         char buf[128];
//         ssize_t r = read(client, buf, sizeof(buf) - 1);
//         printf(buf);
//         if (r > 0)
//         {
//             buf[r] = 0;
//             write(client, "ok\n", 3);
//         }
//         close(client);
//     }
//     close(server_fd);
//     vTaskDelete(NULL);
// }