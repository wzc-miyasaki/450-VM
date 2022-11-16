/**
 *          Client
 * 
 */

#include <iostream>
#include "global.hpp"

#define ATTEMPT_MAX 3

using namespace std;

// username & pass will be separated using ','
void LoginRequest(char* res)
{
    char* buf = res;
    printf("Please enter the username: ");
    char c;
    while(true)
    {
        c = fgetc(stdin);
        if(c == '\n')   break;
        *buf = c;
        ++buf;
    }
    *buf = ',';
    ++buf;
    printf("Please enter the password: ");
    while(true)
    {
        c = fgetc(stdin);
        if(c == '\n')   break;
        *buf = c;
        ++buf;
    }
    *buf = '\0';
}

void CourseQuery(char *res)
{
    char *buf = res;
    char c;
    printf("Please enter the course code to query:");
    while(true)
    {
        c = fgetc(stdin);
        if(c == '\n')   break;
        *buf = c;
        ++buf;
    }
    *buf = ',';
    ++buf;
    printf("Please enter the category (Credit / Professor / Days / CourseName):");
    while(true)
    {
        c = fgetc(stdin);
        if(c == '\n')   break;
        *buf = c;
        ++buf;
    }
    *buf = '\0';
}

void GetClientPort(int skfd, int *_dst)
{
    // get client socket port number
    sockaddr_in clientAddr;
    socklen_t c_addr_len = sizeof(clientAddr);
    int status = getsockname(skfd, (sockaddr*)&clientAddr, &c_addr_len);
    if(status == -1 )
        printf("getsockname failed: %s\n", strerror(errno));
    else
        *_dst = (int) ntohs(clientAddr.sin_port);
}

void extractUsername(const char *_src, char *_dst, size_t dstSz)
{
    memset(_dst, 0, dstSz);
    int idx = 0;
    int range = strlen(_src);
    while(idx < range)
    {
        char c = *(_src + idx);
        if(c == ',')
            break;
        else    
            *(_dst+idx) = c;
        ++idx;
    }

}


int main()
{
    int status;
    int clientPort;

    // create socket
    int skfd = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(TCP_PORT_MAIN);
    
    // connect to remote server 
    status = connect(skfd, (sockaddr*) &serverAddr, sizeof(serverAddr));
    if(status == -1)
    {
        perror("client cannot connect to remote server\n");
        exit(1);
    }

    printf("The client is up and running\n");
    // get client socket Port information
    GetClientPort(skfd, &clientPort);

    // send to server and wait for response
    char readBuf[BUFSIZE_DEFAULT] = {0};
    char sendBuf[BUFSIZE_DEFAULT] = {0};
    char c_code[BUFSIZE_DEFAULT] ;
    char c_type[BUFSIZE_DEFAULT] ;
    char user[20];

    int FailedAttempts = ATTEMPT_MAX;
    bool needAuth = true;
    const char* succeedMsg = "Auth_Succeed";
    

    while(true)
    {
        while(needAuth && FailedAttempts > 0){
            LoginRequest(sendBuf);
            
            // send to server
            send(skfd, sendBuf, strlen(sendBuf)+1, 0);
            extractUsername(sendBuf, user, sizeof(user)); 

            // read from server
            readData(skfd, readBuf, BUFSIZE_DEFAULT-1, BUFSIZE_DEFAULT);
            
            if(strcmp(readBuf, "2")==0)
            {
                needAuth = false;
                send(skfd, succeedMsg, strlen(succeedMsg)+1, 0);
                printf("%s received the result of authentication using TCP over port %d. Authentication is successful\n", user, clientPort);
                break;
            }
            else if(strcmp(readBuf, "1") == 0){
                printf("%s received the result of authentication using TCP over port %d. Authentication failed: Password does not match\n", user, clientPort);
            }
            else if(strcmp(readBuf, "0") == 0){
                printf("%s received the result of authentication using TCP over port %d. Authentication failed: Username Does not exist\n", user, clientPort);
            }

            --FailedAttempts;
            printf("Attempts remaining:%d\n", FailedAttempts);
        }
        
        if(FailedAttempts == 0)
        {
            printf("Authentication Failed for 3 attempts. Client will shut down.");
            break;
        }

        // course info query
        memset(sendBuf, 0, BUFSIZE_DEFAULT);
        memset(c_code, 0, sizeof(c_code));
        memset(c_type, 0, sizeof(c_type));
        CourseQuery(sendBuf);
        AnalyzeQuery(sendBuf, c_code, c_type);
        
        send(skfd, sendBuf, strlen(sendBuf)+1, 0);
        printf("%s sent a request to the main server.\n", user);

        readData(skfd, readBuf, BUFSIZE_DEFAULT-1, BUFSIZE_DEFAULT);
        printf("The client received the response from the Main server using TCP over port %d.\n", clientPort);
        if(strlen(readBuf) == 0)  
            printf("Didn’t find the course: %s.\n", c_code);
        else    
            printf("The %s of %s is %s.\n", c_type, c_code, readBuf);
        printf("\n\n-----Start a new request-----\n");
    }                                                              

    close(skfd);
}