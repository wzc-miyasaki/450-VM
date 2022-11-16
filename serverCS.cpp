/*
 ***** ****** ****** ****** ****** ******
 ******     CS Department Server
 ****** ****** ****** ****** ****** ******
 */

#include <iostream>
#include "global.hpp"
#include "backend.hpp"
#include "courseinfo.hpp"
#include "courseQuery.hpp"

void Response(int _fd, const char* queryResult)
{
    addrinfo hint, *mainS_addr;
    memset(&hint, 0, sizeof(hint));
    hint.ai_family = AF_INET;
    hint.ai_socktype = SOCK_DGRAM;
    if((getaddrinfo(LOCALHOST, UDP_PORT_MAIN, &hint, &mainS_addr))!=0)
    {
        perror("credential server cannot get main server address");
    } else {
        sendto(_fd, queryResult, strlen(queryResult)+1, 0,  mainS_addr->ai_addr, mainS_addr->ai_addrlen);
    }
}

int main()
{
    int sockfd;
    int readNum;

    sockfd = createUDPServerSocket(UDP_PORT_CS);
    if (sockfd == 0)
    {
        perror("failed to initialize a UDP socket");
        return -1;
    }

    printf("The ServerCS is up and running using UDP on port %s\n", UDP_PORT_CS);

    sockaddr_storage clientAddr;
    socklen_t c_addr_sz = sizeof(clientAddr);
    char buf[BUFSIZE_DEFAULT];
    char c_code[BUFSIZE_DEFAULT];
    char c_type[BUFSIZE_DEFAULT];
    CourseQuery cQuery("cs.txt");

    while (true)
    {
        if ((readNum = recvfrom(sockfd, buf, BUFSIZE_DEFAULT-1, 
                                0, 
                                (struct sockaddr *)&clientAddr, &c_addr_sz)) == -1)
        {
            perror("serverCS recvfrom");
            exit(1);
        }

        // Parse the Query
        memset(c_code, 0, sizeof(c_code));
        memset(c_type, 0, sizeof(c_type));
        AnalyzeQuery(buf, c_code, c_type);
        printf("The ServerCS received a request from the Main Server about the %s of %s.\n", c_type, c_code);

        // Search the course Information
        memset(buf, 0, sizeof(buf));
        cQuery.Search(c_code, c_type, buf);

        // show the query result
        if(strlen(buf) == 0) {
            printf("Didn’t find the course: %s.\n", c_code);
        } else {
            printf("The course information has been found: The %s of %s is %s.\n",
                c_type, c_code, buf);
        }

        // send query result to main server
        Response(sockfd, buf);
        printf("The ServerCS finished sending the response to the Main Server.\n\n");
    }

    close(sockfd);
    return 0;
}
