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
    if(this->clientDescriptor > 0){
        close(this->clientDescriptor);
        this->clientDescriptor = -1;
    }
}

bool net::Conn::IsClosed() { return this->clientDescriptor < 0; }
