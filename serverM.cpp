/*
 ***** ****** ****** ****** ****** ******
 ******     Main Server
 ****** ****** ****** ****** ****** ******
 */

#include <iostream>
#include "serverM.hpp"


MainSev:: MainSev() : _buf("")
{
    backends.addSub(new ServerC(LOCALHOST, UDP_PORT_C));
    backends.addSub(new ServerCS(LOCALHOST, UDP_PORT_CS));
    backends.addSub(new ServerEE(LOCALHOST, UDP_PORT_EE));
}

MainSev:: ~MainSev()
{
    if(fd_u != -1)  close(fd_u);
    if(fd_t != -1)  close(fd_t);
    if(listening != -1) close(listening);
}

void 
MainSev:: InitListen()
{
    int status;
    listening = socket(AF_INET, SOCK_STREAM, 0);
    if (listening == -1)
    {
        perror("Listen Socket Failed To Create");
        exit(1);
    }

    struct sockaddr_in hint;
    memset(&hint, 0, sizeof(hint));
    hint.sin_family = AF_INET;
    hint.sin_addr.s_addr = inet_addr(LOCALHOST);
    hint.sin_port = htons(TCP_PORT_MAIN);

    status = bind(listening, (sockaddr *)&hint, sizeof(hint));
    if (status == -1)
    {
        perror("Main Server TCP socket bind failed\n");
        close(listening);
        exit(1);
    }
}

void 
MainSev:: EncryptCharArrHp(char *c)
{
    int ascii_val = (int) *c;
    bool isUpper = ascii_val >= 65 && ascii_val <= 90;
    bool isLower = ascii_val >= 97 && ascii_val <= 122;
    bool isNum  = ascii_val >= 48 && ascii_val <= 57;
    if(isUpper){
        ascii_val += 4;
        ascii_val = ascii_val - 90 <= 0 ? ascii_val : ascii_val-26;
    } else if (isLower) {
        ascii_val += 4;
        ascii_val = ascii_val - 122 <= 0 ? ascii_val : ascii_val-26;
    } else if (isNum) {
        ascii_val += 4;
        ascii_val = ascii_val - 57 <= 0 ? ascii_val : ascii_val-10;
    }
    *c = ascii_val; 
}

void 
MainSev:: Encrypt(char *arr, int sz)
{
    char *tmp = arr;
    for(int idx=0; idx<sz; ++idx)
    {
        EncryptCharArrHp(tmp);
        ++tmp;
    }
}

void 
MainSev:: InitUDPServer(const char* port)
{
    fd_u = createUDPServerSocket(port);
    if (fd_u == -1)
    {
        perror("Main Server failed to initialize a UDP socket");
    }
}

void
MainSev:: InitHint_udp()
{
    memset(&hint, 0, sizeof(hint));
    hint.ai_family = AF_INET;
    hint.ai_socktype = SOCK_DGRAM;
}

void 
MainSev:: ReadTCP(char* buf, size_t sz)
{
    memset(buf, 0, sz);
    read(fd_t, buf, sz);
}

int 
MainSev:: WhatDept(const char* s)
{
    if(strlen(s) < 2)
        return 0;
    int type = s[0] + s[1];
    switch(type)
    {
        case 138:
            return msg_for_EE;
        case 150:
            return msg_for_CS;
        default:
            return msg_undefined;
    }
}

void 
MainSev:: UpdateUserName(const char* _src)
{
    memset(username, 0, sizeof(username));
    int idx = 0;
    int range = (int) strlen(_src);
    char *pt = username;
    while(idx < range){

        char c = *(_src+idx);
        if(c == ',')
            break;
        *pt = c;
        ++ pt;
        ++ idx;
    }
}

void 
MainSev:: BuildTCPConnection()
{
    InitListen();
    listen(listening, 1);
    printf("The main server is up and running\n");
    
    struct sockaddr clientAddr;
    socklen_t addrLen = sizeof(clientAddr);
    memset(&clientAddr, 0, sizeof(addrLen));
    fd_t = accept(listening, &clientAddr, &addrLen);
    if (fd_t == -1)
    {
        perror("Failed to create a client socket");
        close(listening);
    }
}

void 
MainSev:: AuthorizeCredential()
{
    int read_byte;
    char readBuf[BUFSIZE_DEFAULT] = {0};
    char sendBuf[BUFSIZE_DEFAULT] = {0};
    
    while(true){
        read_byte = readData(fd_t, readBuf, BUFSIZE_DEFAULT - 1, BUFSIZE_DEFAULT);
        if (read_byte < 1)
            break;
        
        if(strcmp(readBuf, "Auth_Succeed")==0)
            break;

        UpdateUserName(readBuf);
        printf("The main server received the authentication for %s using TCP over port %u.\n", username, TCP_PORT_MAIN);
        
        Encrypt(readBuf, read_byte-1);  //not include the null character
        _buf.setContent(readBuf);
        _buf.setType(msg_for_Cred);
        backends.NotifyAll(_buf);
        
        sockaddr_storage credAddr;
        socklen_t cred_addr_sz = sizeof(credAddr);
        read_byte = recvfrom(fd_u, sendBuf, BUFSIZE_DEFAULT-1, 0, 
                                (struct sockaddr *)&credAddr, &cred_addr_sz);
        if (read_byte < 1)
            perror("failed to received the authentication result\n");
        else
        {
            printf("The main server received the result of the authentication request from ServerC using UDP over port %s\n", UDP_PORT_MAIN);
            printf("The main server sent the authentication result to the client\n");
            send(fd_t, sendBuf, strlen(sendBuf)+1, 0);
        }

    }
}

void
MainSev:: CourseInfoQuery()
{
    int read_byte;
    int buf_sz = 100;
    char buf[buf_sz];
    char c_code[20];
    char c_type[20];
    while(true)
    {
        ReadTCP(buf, buf_sz);
        if(strlen(buf) == 0)
            break;
        
        memset(c_code, 0, sizeof(c_code));
        memset(c_type, 0, sizeof(c_type));
        AnalyzeQuery(buf, c_code, c_type);
        printf("The main server received from %s to query course %s about %s using TCP over port %u.\n", 
                username, c_code, c_type, TCP_PORT_MAIN); 

        Msg_type deptType = (Msg_type) WhatDept(buf);

        if(deptType != msg_undefined)
        {
            _buf.setContent(buf);
            _buf.setType((Msg_type) WhatDept(buf));
            backends.NotifyAll(_buf);

            sockaddr_storage deptAddr;
            socklen_t addr_sz = sizeof(deptAddr);
            memset(buf, 0, buf_sz);
            read_byte = recvfrom(fd_u, buf, buf_sz - 1, 0, 
                                    (struct sockaddr *)&deptAddr, &addr_sz);
            if(read_byte < 1)
                perror("failded to receive query result");

        } else {
            memset(buf, 0, buf_sz);
        }

        printf("The main server sent the query information to the client.\n");
        send(fd_t, buf, strlen(buf)+1, 0);
    }
}


int main()
{
    MainSev sevM_ctrl;
    sevM_ctrl.InitUDPServer(UDP_PORT_MAIN);
    sevM_ctrl.BuildTCPConnection();

    sevM_ctrl.AuthorizeCredential();
    sevM_ctrl.CourseInfoQuery();

    // free resources
    
    return 0;
}