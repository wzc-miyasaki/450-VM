#include "backend.hpp"

using namespace std;

Message::Message(const char *buf) : content(buf)
{}

void
Message:: setContent(const char* buf)
{
    content = buf;
}

void 
Message:: setType(Msg_type _t)
{
    t = _t;
}

// Backend Sv Observer
void 
BackendSv::addSub(SubBackend* serv)
{
    subscribers.push_back(serv);
}

void 
BackendSv::remvSub(int id)
{
    for (auto idx = subscribers.begin(); idx < subscribers.end(); idx++)
    {
        if ((*idx)->hasID(id))
        {
            delete (*idx);
            subscribers.erase(idx);
            break;
        }
    }
}

void 
BackendSv::remvAll()
{
    for(auto x: subscribers)
        delete x;
    subscribers.clear();
}

void 
BackendSv:: NotifyAll(Message &msg)
{
    for(SubBackend* x : subscribers)
    {
        x->Handle(msg);
    }
}



// backend server subscribers
SubBackend:: SubBackend(const char* ip, const char* port) : _ip(ip) , _port(port)
{
    fd = CreateUDPClientSocket(ip, port);
    
    addrinfo hint;
    memset(&hint, 0, sizeof(hint));
    hint.ai_family = AF_INET;
    hint.ai_socktype = SOCK_DGRAM;
    if((getaddrinfo(_ip, _port, &hint, &addr)) != 0)
    {
        printf("failed to get address info\n");
    }

    if(addr == NULL)
        printf("failed to get address info\n");

    if(fd == -1)
        printf("%s [%s] failed to create\n", ip, port);
}

SubBackend:: ~SubBackend() 
{}

bool 
SubBackend::hasID(int id)
{
    return id == this->fd;
}


ServerC:: ServerC(const char* ip, const char* port) : SubBackend(ip, port) 
{
    t = msg_for_Cred;
}

void
ServerC:: Handle(Message& msg)
{
    // include null terminator
    if(t == msg.t)
    {
        if( (sendto( fd, msg.content, strlen(msg.content)+1, 0, addr->ai_addr, addr->ai_addrlen )) > 0)
            printf("The main server sent an authentication request to serverC.\n");
        else
            perror("Main Server Failed to send message to serverC");
    }
}


ServerCS:: ServerCS(const char* ip, const char* port) : SubBackend(ip, port) 
{
    t = msg_for_CS;
}

void
ServerCS:: Handle(Message& msg)
{
    if(t == msg.t)
    {
        if( (sendto( fd, msg.content, strlen(msg.content)+1, 0, addr->ai_addr, addr->ai_addrlen )) > 0)
            printf("The main server sent a request to serverCS\n");
        else
            perror("Main Server Failed to send message to serverCS");
    }
}


ServerEE:: ServerEE(const char* ip, const char* port) : SubBackend(ip, port) 
{
    t = msg_for_EE;
}

void
ServerEE:: Handle(Message& msg)
{
    if(t == msg.t)
    {
        if( (sendto( fd, msg.content, strlen(msg.content)+1, 0, addr->ai_addr, addr->ai_addrlen )) > 0)
            printf("The main server sent a request to serverEE\n");
        else
            perror("Main Server Failed to send message to serverEE");
    }
}