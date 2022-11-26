/**
 *          Client
 * 
 */

#include <iostream>
#include <vector>
#include "global.hpp"

// Maximum attempts
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

// if it's single query, return 1;
// if it's multiple course query,  return 2;
int CourseQuery(char *res)
{
    int queryStatus = 1;
    bool isMultiple = false;
    char *buf = res;
    char c;
    printf("Please enter the course code to query:");
    while(true)
    {
        c = fgetc(stdin);
        if(c == '\n')   break;
        if(c == ' ') {
            isMultiple = true;
            queryStatus= 2;
        }
        *buf = c;
        ++buf;
    }

    if(!isMultiple) 
    {
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

    return queryStatus;
}

// get tcp client port number, which is dynamically assigned by operating system
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

// extract username field from the buffer "_dst"
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

// < multiple course queries > scenario:  display all query results
void DisplayMultRes(std::vector<std::string>& courseList, const char* res, const std::string& symbol)
{
    printf("CourseCode: Credits, Professor, Days, Course Name\n");
    std::string src(res);
    auto head = 0;
    auto tail = src.find(symbol, 0);
    int courseIdx = 0;
    while(tail != std::string::npos)
    { 
        std::cout << courseList[courseIdx] << ": "
                << src.substr(head, tail-head) << "\n";
        head = tail + symbol.length();
        tail = src.find(symbol, head);
        ++courseIdx;
    }
}

// < multiple course queries > scenario:  extract multiple course codes from the "list", and saved them in c++ vector datatype "_dst"
void ExtractMultipleCode(const char* lists, std::vector<std::string>& _dst)
{
    std::string src(lists);
    std::string tmp = "";
    src.push_back(' '); 
    for(char c : src){
        if(c == ' '){
            if(!tmp.empty()){
                _dst.push_back(tmp);
                tmp.clear();
            }  
        }else{
            tmp.push_back(c);
        }
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
            
            // send authentication to server
            send(skfd, sendBuf, strlen(sendBuf)+1, 0);
            extractUsername(sendBuf, user, sizeof(user)); 

            // read authentication result from server
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
            printf("Authentication Failed for 3 attempts. Client will shut down.\n");
            break;
        }

        // course info query
        int q_type;
        memset(sendBuf, 0, BUFSIZE_DEFAULT);
        memset(c_code, 0, sizeof(c_code));
        memset(c_type, 0, sizeof(c_type));
        q_type = CourseQuery(sendBuf);
        AnalyzeQuery(sendBuf, c_code, c_type);
        send(skfd, sendBuf, strlen(sendBuf)+1, 0);
        printf("\n");


        switch(q_type)
        {
            case 1: printf("%s sent a request to the main server.\n", user); break;
            case 2: printf("%s sent a request with multiple CourseCode to the main server.\n", user); break;
            default: printf("\n"); break;
        }


        // display query result
        readData(skfd, readBuf, BUFSIZE_DEFAULT-1, BUFSIZE_DEFAULT);
        printf("The client received the response from the Main server using TCP over port %d.\n", clientPort);
        if(strlen(readBuf) == 0)  
            printf("Didn’t find the course: %s.\n", c_code);
        else    
        {
            if(q_type == 1)
                printf("The %s of %s is %s.\n", c_type, c_code, readBuf);
            if(q_type == 2)
            {
                std::vector<string> courseList;
                ExtractMultipleCode(c_code, courseList);
                DisplayMultRes(courseList, readBuf, "#");
            }
        }
        printf("\n\n-----Start a new request-----\n");
    }                                                              

    close(skfd);
}