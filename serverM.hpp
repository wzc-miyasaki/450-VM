#include <vector>
#include <string>
#include "global.hpp"
#include "backend.hpp"

class MainSev
{
    public:
        MainSev();
        ~MainSev();
        void InitUDPServer(const char* port);
        void InitListen();           // create listen socket for TCP handshaking
        void BuildTCPConnection();    
        bool AuthorizeCredential();  // credential authentication implementation
        void CourseInfoQuery();      // course query implementation

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
        
        void Encrypt(char *arr, int sz);    // it encrypts a char array "arr" with a helper function
        void EncryptCharArrHp(char *c);     // encryption algorithm implementation: encrypt a char array
        void InitHint_udp();                // initialize the attribute "hint" 
        void ReadTCP(char* buf,  size_t z); // it reads from TCP socket, and save data to a buffer "buf"
        int  WhatDept(const char* s);       // provide a course code "s" , and it returns Department category
        void UpdateUserName(const char* _src);  //  when main server handshake with new client, it updates the attribute "username"
        
        /**
         * Update the Message attribute "_buf" with new query "query" and message type "dept" which stands for
         * department category. Main server forward message to backend server, and waits to receive query result.
         * The result will be saved to the buffer "_dst" that has "_dst_size" bytes;
         */
        void QueryDept(char* query, Msg_type dept, char* _dst, int _dst_size);

        void ExtractMultipleCode(const char* lists, std::vector<std::string>& _dst);    // extract multiple course codes from the query
};