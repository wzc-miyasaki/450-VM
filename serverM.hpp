#include <vector>
#include "global.hpp"
#include "backend.hpp"

class MainSev
{
    public:
        MainSev();
        ~MainSev();
        void InitUDPServer(const char* port);
        void BuildTCPConnection();
        void AuthorizeCredential();
        void CourseInfoQuery();

    private:
        // addrinfo *servList;
        int fd_u;
        int fd_t;
        int listening; 
        BackendSv backends; 
        Message _buf;
        addrinfo hint;
        addrinfo *credAddr;
        char username[20];
        
        void InitListen();
        void Encrypt(char *arr, int sz);
        void EncryptCharArrHp(char *c);
        void InitHint_udp();
        void ReadTCP(char* buf,  size_t z);
        int  WhatDept(const char* s);
        void UpdateUserName(const char* _src);
};