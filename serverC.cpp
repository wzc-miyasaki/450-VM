/*
 ***** ****** ****** ****** ****** ******
 ******     Credential Server
 ****** ****** ****** ****** ****** ******
 */

#include <iostream>
#include "global.hpp"
#include "backend.hpp"


// it takes a char array "info" that holds the information <username,password>;
// it extracts the "username" to the buffer "_u" and "password" to the buffer "_p"
void getUserAndPass(const char *info, char *_u, char *_p)
{
    char *pt = _u;
    int end = strlen(info);
    int idx = 0;
    while(idx < end)
    {
        char c = *(info + idx);

        // deal with Windows Platfrom, which has "\r\n" at the end of line
        if(c == '\r' || c == '\n')
            break;
        else if(c == ',')    
            pt = _p;
        else {
            *pt = c;
            ++pt;
        }

        ++idx;
    }
}


// return 0:  username does not exist
// return 1:  valid username but wrong password
// return 2:  correct username and password
int verifyCred(char *info)
{
    int status = 0;
    char user[64] = {0};
    char pass[64] = {0};
    char buf[64];
    char verifiedUser[64] = {0};
    char verifiedpass[64] = {0};

    getUserAndPass(info, user, pass);
    FILE *txt;
    txt = fopen("cred.txt", "r");
    if(txt == NULL)
        return 0;
    
    while(fgets(buf, 100, txt) != NULL)
    {
        memset(verifiedUser, 0, 64);
        memset(verifiedpass, 0, 64);
        getUserAndPass(buf, verifiedUser, verifiedpass);

        if(strcmp(user, verifiedUser)==0)
            ++status;

        if(status > 0)
        {
            if(strcmp(pass, verifiedpass)==0)
                ++status;
            break;
        }
    }

    fclose(txt);
    return status;
}



int main()
{
    int sockfd;
    int readNum;
    int sendNum;

    sockfd = createUDPServerSocket(UDP_PORT_C);
    if (sockfd == 0)
    {
        perror("failed to initialize a UDP socket");
        return -1;
    }

    printf("The ServerC is up and running using UDP on port <%s>\n", UDP_PORT_C);

    sockaddr_storage clientAddr;
    socklen_t c_addr_sz = sizeof(clientAddr);
    char buf[BUFSIZE_DEFAULT];


    while (true)
    {
        // listen at Credential Server Port
        if ((readNum = recvfrom(sockfd, buf, BUFSIZE_DEFAULT-1, 
                                0, 
                                (struct sockaddr *)&clientAddr, &c_addr_sz)) == -1)
        {
            perror("serverC recvfrom");
            exit(1);
        }

        printf("The ServerC received an authentication request from the Main Server.\n");
        int authStatus = verifyCred(buf);

        // send authentication result to server
        addrinfo hint, *mainS_addr;
        memset(&hint, 0, sizeof(hint));
        hint.ai_family = AF_INET;
        hint.ai_socktype = SOCK_DGRAM;
        if((getaddrinfo(LOCALHOST, UDP_PORT_MAIN, &hint, &mainS_addr))!=0)
        {
            perror("credential server cannot get main server address");
            break;
        }

        std::string res = std::to_string(authStatus);   //results: 0, 1 or 2
        if((sendNum = sendto(sockfd, res.c_str(), 1, 0, mainS_addr->ai_addr, mainS_addr->ai_addrlen)) < 0)
            perror("The ServerC failed to send result");
        else
            printf("The ServerC finished sending the response to the Main Server.\n\n");

    }

    close(sockfd);
    return 0;
}
