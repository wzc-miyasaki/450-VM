#ifndef _GLOBAL_HPP
#define _GLOBAL_HPP


#define LOCALHOST "127.0.0.1"

/**
 *      Static Port Requirment
 */
#define UDP_PORT_C  "21344"
#define UDP_PORT_CS "22344"
#define UDP_PORT_EE "23344"
#define UDP_PORT_MAIN "24344"
const unsigned short int TCP_PORT_MAIN  = 25344;

#define BUFSIZE_DEFAULT 4096
 

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h> 
#include <sys/types.h> 
#include <netinet/in.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <sys/wait.h>

// credential authentication status enumerator.  
enum AuthFlag {
    USER_NOTFOUND = 0,
    WRONG_PASS = 1,
    SUCCEED = 2
};


int readData(int fd, char *buf, int bufSize, size_t readNum);
int createUDPServerSocket(const char* port);    
int CreateUDPClientSocket(const char* ip, const char *port);
int getAddrinfoAtUDPPort(const char* port, addrinfo* res);
void AnalyzeQuery(const char *q, char *id, char *key);




#endif