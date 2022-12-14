#ifndef BACKEND_HPP
#define BACKEND_HPP
#include "global.hpp"
#include <vector>
#include <string>


class SubBackend;
class BackendSv;

// Define the Message Type
enum Msg_type {
    msg_undefined=0,
    msg_for_Cred=1, 
    msg_for_CS=2, 
    msg_for_EE=3,
    msg_for_multiple=10
};


class Message
{
    public:
        Msg_type t;
        const char* content;
        Message(const char* buf);
        void setContent(const char* buf);
        void setType(Msg_type _t);
};


/**     Observer Design Pattern
 */

// Subject Class
class BackendSv
{
public:
    BackendSv() {}
    ~BackendSv() { remvAll(); }
    void addSub(SubBackend* serv);
    void remvSub(int id);
    void remvAll();
    void NotifyAll(Message &msg);
private:
    std::vector<SubBackend*> subscribers;
};


// Observer Class
class SubBackend
{
    public:
        SubBackend(const char* ip, const char* port);
        virtual ~SubBackend();
        bool hasID(int id);
        virtual void Handle(Message& msg) = 0;

        int fd;     // server socket
        Msg_type t;
        const char* _ip;
        const char* _port;
        addrinfo *addr;
};



// define 3 backend server subclasses 
class ServerC : public SubBackend
{
    public:
        ServerC(const char* ip, const char* port);
        ~ServerC(){ close(fd); }
        virtual void Handle(Message& msg) override;
};

class ServerCS : public SubBackend
{
    public:
        ServerCS(const char* ip, const char* port);
        ~ServerCS(){ close(fd); }
        virtual void Handle(Message& msg) override;
};

class ServerEE : public SubBackend
{
    public:
        ServerEE(const char* ip, const char* port);
        ~ServerEE(){ close(fd); }
        virtual void Handle(Message& msg) override;
};





#endif