#include "global.hpp"

// reads data from the socket "fd" , and save content to a buffer "buf".
// it reads "readNum" bytes of data.  
int readData(int fd, char *buf, int bufSize, size_t readNum)
{
    int readSz;
    readSz = recv(fd, buf, readNum, 0); 
    if(readSz < bufSize)
        buf[readSz] = '\0';
    return readSz;
}


// Based on Beej's Book:  Chapter 6 (6.3 Datagram sockets)
// create UDP server socket with help of getaddrinfo. 
int createUDPServerSocket(const char* port)
{
    int status, sockfd;
    addrinfo hint, *addrlist, *p;
    memset(&hint, 0, sizeof(hint));
    hint.ai_family = AF_INET;
    hint.ai_socktype = SOCK_DGRAM;
    hint.ai_flags = AI_PASSIVE;

    if((status=getaddrinfo(LOCALHOST, port, &hint, &addrlist)) != 0)
    {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
        return -1;
    }

    for (p = addrlist; p != NULL; p = p->ai_next)
    {
        if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
        {
            perror("listener: socket");
            continue;
        }

        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1)
        {
            close(sockfd);
            perror("listener: bind");
            continue;
        }

        break;
    }

    freeaddrinfo(addrlist);

    if(p == NULL) {
        perror("Failed to create & bind socket\n");
        return -1;
    }

    return sockfd;
}


// Based on Beej's Book:  Chapter 6 (6.3 Datagram sockets)
// create UDP client socket with help of getaddrinfo. 
int CreateUDPClientSocket(const char* ip, const char *port)
{
    addrinfo hint, *servList;
    memset(&hint, 0, sizeof(hint));
    hint.ai_family = AF_INET;
    hint.ai_socktype = SOCK_DGRAM;

    int status;
    status = getaddrinfo(ip, port, &hint, &servList);
    
    if(status == -1)
        return -1;
    
    if(servList == NULL)
        return -1;
    
    int fd = socket(servList->ai_family, servList->ai_socktype, servList->ai_protocol);
    freeaddrinfo(servList);
    
    return fd;
}


int getAddrinfoAtUDPPort(const char* port, addrinfo* res)
{
    addrinfo hint;
    addrinfo* addrlist = (addrinfo*) malloc(sizeof(addrinfo));
    memset(&hint, 0, sizeof(addrinfo));
    hint.ai_family = AF_INET;
    hint.ai_socktype = SOCK_DGRAM;

    int status;
    if((status=getaddrinfo(LOCALHOST, port, &hint, &addrlist)) != 0)
    {
        perror("getUDPSocketAddrAtPort failed to get address\n");
        return -1;
    }

    if(addrlist != NULL)
    {
        res = addrlist;
        return 0;
    }

    return -1;
}


// Analyze a course Query, which has only 2 parts: course id "id" and information category "key"
void 
AnalyzeQuery(const char *q, char *id, char *key)
{
    char *pt = id;
    int end = strlen(q);
    int idx = 0;
    while(idx < end)
    {
        char c = *(q + idx);
        if(c == '\r' || c == '\n')
            break;
        else if(c == ',')    
            pt = key;
        else {
            *pt = c;
            ++pt;
        }

        ++idx;
    }   
}

